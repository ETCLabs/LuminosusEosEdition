#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Name: luminosus.next
# Innovative node-based lighting software.
# Author: Tim Henning
#
# Copyright 2015 Tim Henning
#
# Luminosus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

import socket
import struct
import threading
import random
import time
import numpy as np

version = 2.99


class Test():

    def run_test(self):
        artnet = ArtNet()
        artnet.daemon = True
        artnet.start()
        time.sleep(2)
        print artnet.own_ip
        print artnet.get_nodes()


class ArtNet(threading.Thread):
    """ This class can be used to send ArtNet messages and to receive ArtNet data from the network.
    For simplicity and speed this ArtNet implementation only works with multiplies of 512 channels.
    """
    
    def __init__(self, address=(0, 0), hibernate=False, use_unicast=True, ignore_local_data=True, data_length=512, main=None):
        """ Creates a new ArtNet object that starts listening on the network for ArtNet data.
        :param address: Specifies the subnet on which to send and receive data. (subnet, net)
        :param hibernate: True to not try to send or receive data (e.g. if network isn't ready).
        :param use_unicast: True to use unicast to send data, otherwise broadcast will be used.
        :param ignore_local_data: True to ignore data from the local computer.
        :param data_length: Length of DMX data. Must be a multiple of 512 (= count of universes).
        :return: A new ArtNet object.
        """
        threading.Thread.__init__(self)
        
        # set method parameter values
        subnet, net = address
        if 0 <= subnet <= 15 and 0 <= net <= 127:
            # Subnet must be 0-15, Net 0-127
            self.address = (subnet, net)
        else:
            # Default address is 0, 0 if given address is invalid
            self.address = (0, 0)
        self.hibernate = hibernate
        self.use_unicast = use_unicast
        self.send_dmx_data = None
        self.set_output_mode()  # Changes self.send_dmx_data
        self.ignore_local_data = ignore_local_data
        self.data_length = data_length
        self.last_values = np.zeros(data_length)
        self.socket_open = False
        self.s = None
        
        if data_length % 512:
            # for simplicity and speed this ArtNet implementation only works with multiplies of 512 channels
            self.debug_output("Data length must be a multiple of 512.")
            return
        # prepare data buffer
        self.universe_count = int(data_length / 512)
        self.universes = []
        for i in xrange(16):
            self.universes.append(bytearray([0] * 513))
        self.active_universes = []
        
        # set random ip if ip_check failures
        self.own_ip = "192.168.178.20"
        # random value to identify ip_check_packet
        self.ip_check_id = random.randint(1, 1024)

        # nodes list
        self.nodes = []
        
        # prebuild packages and headers for faster access
        self.header_list = []
        self.ArtPoll_content = None
        self.ip_check_content = None
        self.ArtPollReply_content = None
        self.build_packages()

    @staticmethod
    def debug_output(text, color="info"):
        """ A simple method for colored console output.
        :param text: Message.
        :param color: One of "info" or "error".
        """
        print text

    #                                                ############ Flow Control ###############
    
    def run(self):
        """ The main event loop of this ArtNet object.
        Reacts to changes in network state.
        """
        sleep = time.sleep
        while True:
            # check for network connection
            if not self.hibernate:
                if not self.socket_open:
                    if not self.open_socket():
                        continue
                self.set_own_ip()
                self.ArtPoll()
                self.server()
            # wait to check again for hibernate and possibility to open socket
            sleep(1)
    
    def open_socket(self):
        """ Opens a socket (self.s) and sets self.socket_open to true if it was successful. """
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.s.bind(("", 6454))
            self.s.sendto("ping", ("<broadcast>", 60000))
            self.socket_open = True
            return True
        except IOError as e:
            try:
                self.s.close()
            except IOError:
                pass
            self.socket_open = False
            self.debug_output("Missing network connection.", "error")
            self.debug_output("Error: %s" % e, "error")
            return False
    
    def start_hibernate(self):
        self.debug_output("ArtNet paused.")
        self.hibernate = True
        try:
            self.kill_server()
        except AttributeError:
            pass
        self.set_output_mode()
    
    def stop_hibernate(self):
        self.debug_output("ArtNet continued.")
        self.hibernate = False
        self.set_output_mode()
    
    def close(self):
        self.start_hibernate()
        self.debug_output("ArtNet node stopped.")
    
    #                                                ############ Server ###############
    
    def server(self):
        """ This method listens to ArtNet messages on the network. """
        opcode_ArtDMX = 0x5000
        opcode_ArtPoll = 0x2000
        opcode_ArtPollReply = 0x2100
        opcode_ip_check = 0x6464
        opcode_kill = 0x9999
        ignore_local_data = self.ignore_local_data
        own_ip = None
        recvfrom = self.s.recvfrom
        unpack = struct.unpack
        handle_ArtDMX = self.handle_ArtDMX
        self.debug_output("ArtNet node started.")
        try:
            while True:
                data, addr = recvfrom(4096)
                if data[:8] != 'Art-Net\x00':
                    continue
                opcode = unpack('<H', data[8:10])[0]
                if opcode == opcode_ArtDMX:  # most likely
                    if ignore_local_data and addr[0] == own_ip:
                        continue
                    handle_ArtDMX(data)
                elif opcode == opcode_ArtPoll:
                    self.handle_ArtPoll(addr)
                elif opcode == opcode_ArtPollReply:
                    self.handle_ArtPollReply(addr)
                elif opcode == opcode_ip_check:
                    self.handle_ip_check(data, addr)
                    own_ip = self.own_ip
                elif opcode == opcode_kill:
                    if data[12:] == str("luminosus_kill_signal_%s" % self.ip_check_id):
                        self.debug_output("ArtNet listener received stop signal.")
                        break
                else:
                    self.debug_output("Received unknown package. OpCode: %s" % opcode, "error")
        except IOError as e:
            self.debug_output("IOError: %s" % e, "error")
        finally:
            self.s.close()
            self.socket_open = False

    def handle_ArtDMX(self, data):
        """ ArtDMX is incoming DMX data. The data will be saved in self.universes. """
        p_subnet = ord(data[14]) >> 4
        p_universe = ord(data[14]) - (p_subnet << 4)
        p_net = ord(data[15])
        if p_net != self.address[1] or p_subnet != self.address[0]:
            return False
        data_length = struct.unpack('>H', data[16:18])[0]
        self.universes[p_universe][1:data_length + 1] = [ord(i) for i in data[18:]]

    def handle_ArtPoll(self, addr):
        """ ArtPoll is a message to find out which other ArtNet devices are in the network.
        Sends an ArtPollReply message as an answer. """
        if addr[0] != self.own_ip and addr not in self.nodes:
            self.nodes.append(addr)
        self.ArtPollReply()
    
    def handle_ArtPollReply(self, addr):
        """ ArtPollReply is a message that describes an ArtNet device.
        These devices will be added to self.nodes. """
        if addr[0] != self.own_ip and addr not in self.nodes:
            self.nodes.append(addr)
    
    def handle_ip_check(self, data, addr):
        """ ip_check is a message from this ArtNet implementation to find out the own ip address. """
        if data[12:] == str("luminosus_ip_check_%s" % self.ip_check_id):
            self.own_ip = addr[0]
            self.debug_output("IP of this ArtNet Node: %s" % self.own_ip)
            for addr in self.nodes[:]:
                if addr[0] == self.own_ip:
                    self.nodes.remove(addr)
            self.build_ArtPollReply()
    
    def set_own_ip(self):
        """ Sends an ip_check message to find out the own ip address. """
        self.s.sendto(self.ip_check_content, ('<broadcast>', 6454))
    
    def ArtPoll(self):
        """ Sends an ArtPoll message to find other ArtNet devices. """
        self.s.sendto(self.ArtPoll_content, ("<broadcast>", 6454))
        
    def ArtPollReply(self):
        """ Sends an ArtPollReply message to reply to an ArtPoll message. """
        self.s.sendto(self.ArtPollReply_content, ("<broadcast>", 6454))
    
    def add_artnet_node(self, node_ip):
        """ A method to manual add an ArtNet device to the list of devices.
        :param node_ip: The ip address of the new device.
        :return: True if the device was unknown before.
        """
        if node_ip not in self.nodes:
            self.nodes.append((node_ip, 6454))
            return True
        else:
            return False

    def refresh_nodes(self):
        """ Sends an ArtPoll message to find other ArtNet devices. """
        self.ArtPoll()
        return True

    def kill_server(self):
        """ Sends a message to safely stop the loop that listens on the network. """
        content = []
        # id8, 7byte + 0x00
        content.append("Art-Net\x00")
        # OpCode ArtDMX -> 0x5000, Low Byte first
        content.append(struct.pack('<H', 0x9999))
        # Protocol Version 14, High Byte first
        content.append(struct.pack('>H', 14))
        # luminosus ip check
        content.append("luminosus_kill_signal_%s" % self.ip_check_id)
        content = ''.join(content)
        try:
            self.s.sendto(content, ('<broadcast>', 6454))
        except IOError:
            self.debug_output("Could not stop ArtNet server.", "error")

    #                                                ############ Getter / Setter ###############

    def set_address(self, address):
        """ Set the address to send and receive data (subnet, net).
        Subnet must be between 0 and 15.
        Net must be between 0 and 127.
        :return: True if the address is valid. """
        subnet, net = address
        if 0 <= subnet <= 15 and 0 <= net <= 127:
            self.address = address
            self.build_packages()
            return True
    
    def get_nodes(self):
        """ Returns the known other ArtNet nodes on the network. """
        return self.nodes
    
    def get_subnet_data(self):
        """ Returns the data of all universes in the current subnet. """
        return self.universes
        
    #                                                ############ Send Data ###############
    
    def set_unicast(self, unicast=True):
        """ Set the mode to send the ArtNet messages. """
        self.use_unicast = unicast
        self.set_output_mode()
    
    def set_output_mode(self):
        """ Intern method. """
        if not self.hibernate:
            if self.use_unicast:
                self.send_dmx_data = self.ArtDMX_unicast
            else:
                self.send_dmx_data = self.ArtDMX_broadcast
        else:
            self.send_dmx_data = self.ArtDMX_dummy
        return True

    def ArtDMX_dummy(self, dmxdata, universe=0):
        """ Intern method. """
        pass
    
    def ArtDMX_unicast(self, dmxdata, universe=0):
        """ Intern method to send DMX data using unicast.
        :param dmxdata: 512 0-255 integer values
        :param universe: The target universe of the data.
        """
        content = bytearray(self.header_list[universe])
        # DMX Data
        content.extend(dmxdata)
        # send
        for addr in self.nodes:
            self.s.sendto(content, addr)

    def ArtDMX_broadcast(self, dmxdata, universe=0):
        """ Intern method to send DMX data using broadcast.
        :param dmxdata: 512 0-255 integer values
        :param universe: The target universe of the data.
        """
        content = bytearray(self.header_list[universe])
        # DMX Data
        content.extend(dmxdata)
        # send
        self.s.sendto(content, ('<broadcast>', 6454))
    
    def send_data(self, data):
        """ This is the right method to be used to send DMX data.
        :param data: A numpy array of data_length 0-1 float values.
        :return: True if the data_length was valid.
        """
        length = len(data)
        if length != self.data_length:
            return False
        
        diff = data - self.last_values
        if not diff.any():
            return True
        self.last_values = data.copy()
        
        data = (data * 255).astype("uint8")
        
        for i in xrange(length / 512):
            if not diff[i*512: (i+1)*512].any():
                continue
            universe_data = data[i*512 : (i+1)*512]
            self.send_dmx_data(universe_data, i)
        
    #                                                ############ Header / Packet Contents ############
        
    def build_packages(self):
        """ A method to prebuild packages and headers for faster access. """
        self.build_ArtPoll()
        self.build_ArtPollReply()
        self.build_ip_check()
        
        self.header_list = []
        for u in xrange(16):
            self.header_list.append(self.get_ArtDMX_header(u))
    
    def get_ArtDMX_header(self, universe=0, eternity_port=1):
        """ Builds an ArtDMX header.
        :param universe: The universe.
        :param eternity_port: The eternity port.
        :return: The header data.
        """
        header = []
        # Name, 7byte + 0x00
        header.append("Art-Net\x00")  # 8
        # OpCode ArtDMX -> 0x5000, Low Byte first
        header.append(struct.pack('<H', 0x5000))  # 2
        # Protocol Version 14, High Byte first
        header.append(struct.pack('>H', 14))  # 2
        # Order -> nope -> 0x00
        header.append("\x00")  # 1
        # Eternity Port
        header.append(chr(eternity_port))  # 1
        # Address
        # universe, subnet, net = self.address
        subnet, net = self.address
        header.append(struct.pack('<H', net << 8 | subnet << 4 | universe))  # 2
        # Length of DMX Data per packet, High Byte First
        header.append(struct.pack('>H', 512))
        header = "".join(header)
        header = bytearray(header)
        return header
        
    def build_ArtPoll(self):
        """ Builds an ArtPoll message. """
        content = []
        # Name, 7byte + 0x00
        content.append("Art-Net\x00")
        # OpCode ArtPoll -> 0x2000, Low Byte first
        content.append(struct.pack('<H', 0x2000))
        # Protocol Version 14, High Byte first
        content.append(struct.pack('>H', 14))
        # TalkToMe
        content.append(struct.pack('>H', 0b00000000))
        content.append(chr(0xe0))
        self.ArtPoll_content = "".join(content)
    
    def build_ip_check(self):
        """ Builds an ip_check message. """
        content = []
        # id8, 7byte + 0x00
        content.append("Art-Net\x00")
        # OpCode ArtDMX -> 0x5000, Low Byte first
        content.append(struct.pack('<H', 0x6464))
        # Protocol Version 14, High Byte first
        content.append(struct.pack('>H', 14))
        # luminosus ip check
        content.append("luminosus_ip_check_%s" % self.ip_check_id)
        self.ip_check_content = ''.join(content)
    
    def build_ArtPollReply(self):
        """ Builds an ArtPollReply message. """
        content = []
        # Name, 7byte + 0x00
        content.append("Art-Net\x00")
        # OpCode ArtPollReply -> 0x2100, Low Byte first
        content.append(struct.pack('<H', 0x2100))
        # Protocol Version 14, High Byte first
        # content.append(struct.pack('>H', 14))  # <- not in ArtPollReply
        # IP
        ip = [int(i) for i in self.own_ip.split('.')]
        content += [chr(i) for i in ip]
        # Port
        content.append(struct.pack('<H', 0x1936))
        # Firmware Version
        content.append(struct.pack('>H', 200))
        # Net and subnet of this node
        content.append(chr(self.address[1]))
        content.append(chr(self.address[0]))
        # OEM Code (E:Cue 1x DMX Out)
        content.append(struct.pack('>H', 0x0360))
        # UBEA Version -> Nope -> 0
        content.append(chr(0))
        # Status1
        content.append(struct.pack('>H', 0b11010000))
        # Manufacture ESTA Code
        content.append('LL')
        # Short Name
        content.append('luminosus-server3\x00')
        # Long Name
        content.append('luminosus-server3_ArtNet_Node' + '_' * 34 + '\x00')
        # stitch together
        self.ArtPollReply_content = ''.join(content)

if __name__ == "__main__":
    Test().run_test()
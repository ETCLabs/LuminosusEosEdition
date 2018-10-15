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

import glob
import os
import time
from kivy.uix.floatlayout import FloatLayout
from kivy.clock import Clock
from kivy.app import App
from kivy.uix.video import Video
from kivy.clock import Clock

import sys

try:
    from data.output_data import artnet
except ImportError:
    import artnet


class MediaServer(App):

    ARTNET_ADDRESS = (0, 0, 0)  # (universe, subnet, net)
    START_CHANNEL = 500

    def __init__(self, **kwargs):
        super(MediaServer, self).__init__(**kwargs)
        self.gui = None
        self.video = None
        self.videos = None
        self.last_channels = [0] * 12
        self.artnet = None
        self.artnet_initialized = False
        self.init_artnet()
        self.index_videos()
        Clock.schedule_once(self.start_updating, 2)

    def start_updating(self, dt=None):
        Clock.schedule_interval(self.update, 1 / 30.)

    def build(self):
        self.gui = MediaPlayerGui()
        return self.gui

    def index_videos(self):
        path = "../media_server_videos/"
        path = os.path.expanduser(path)
        if not os.path.isdir(path):
            print "There is no folder %s." % path
            sys.exit()
        filenames = glob.glob(path + "*")
        filenames = sorted(filenames)
        print "--- Channel %s for BLACK" % (MediaServer.START_CHANNEL + 0)
        print "--- Channel %s for CONTINUE" % (MediaServer.START_CHANNEL + 1)
        print "--- Channel %s for PAUSE" % (MediaServer.START_CHANNEL + 2)
        for i, name in enumerate(filenames):
            print "--- Channel %s for video %s" % (MediaServer.START_CHANNEL + 3 + i, name)
        self.videos = filenames

    def update(self, dt=None):
        channels = self.artnet.universes[MediaServer.ARTNET_ADDRESS[0]][MediaServer.START_CHANNEL: MediaServer.START_CHANNEL + 9]
        if self.last_channels[0] < 127 < channels[0]:
            self.black()
        elif self.last_channels[1] < 127 < channels[1]:
            self.continue_video()
        elif self.last_channels[2] < 127 < channels[2]:
            self.pause_video()
        else:
            for i in range(6):
                if self.last_channels[i+3] < 127 < channels[i+3]:
                    self.play_video(i)
                    break
        self.last_channels = channels

    def play_video(self, index):
        print "PLAY " + str(index)
        self.black()
        if len(self.videos) <= index:
            return
        src = self.videos[index]
        self.video = Video(source=src)
	self.video.allow_stretch = True
	self.video.state = "play"
        Clock.schedule_once(self.start_video, 0.2)

    def start_video(self, a=None, b=None):
	if self.video:
            self.gui.video_container.add_widget(self.video)

    def continue_video(self):
        print "CONTINUE"
        if self.video:
            self.video.state = "play"

    def pause_video(self):
        print "PAUSE"
        if self.video:
            self.video.state = "pause"

    def black(self):
        if self.video:
            print "BLACK"
            self.video.state = "stop"
        self.gui.video_container.clear_widgets()
        self.video = None

    def init_artnet(self):
        if self.artnet_initialized:
            return False
        self.artnet = artnet.ArtNet(MediaServer.ARTNET_ADDRESS[1:3], ignore_local_data=False)
        self.artnet.daemon = True
        self.artnet.start()
        self.artnet_initialized = True


class MediaPlayerGui(FloatLayout):
    pass


if __name__ in ('__android__', '__main__'):
    main = MediaServer()
    main.run()

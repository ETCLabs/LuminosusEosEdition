{
    "blocks": [
        {
            "focused": false,
            "internalState": {
                "channel": 1,
                "target": 1,
                "useDefaultChannel": true
            },
            "name": "MIDI Program In",
            "posX": 638.46156188016903,
            "posY": 139.2307743377236,
            "uid": "14641671729500"
        },
        {
            "focused": false,
            "internalState": {
                "channel": 1,
                "target": 2,
                "useDefaultChannel": true
            },
            "name": "MIDI Program In",
            "posX": 641.53848506995291,
            "posY": 225.3846236516741,
            "uid": "14641671733478"
        },
        {
            "focused": false,
            "internalState": {
                "cueList": 1,
                "cueNumber": 2
            },
            "name": "Eos Cue",
            "posX": 431.53847736719854,
            "posY": 225.3846236516741,
            "uid": "14641672117464"
        },
        {
            "focused": false,
            "internalState": {
                "cueList": 1,
                "cueNumber": 1
            },
            "name": "Eos Cue",
            "posX": 431.53847736719854,
            "posY": 139.2307743377236,
            "uid": "14641672126281"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Map MIDI program 1 and 2 \nto Cue 1 and 2 in Cue List 1:"
            },
            "name": "Notes",
            "posX": 128.46154317347978,
            "posY": 176.92308341257694,
            "uid": "14641672301491"
        },
        {
            "focused": true,
            "internalState": {
                "channel": 1,
                "target": 3,
                "useDefaultChannel": true
            },
            "name": "MIDI Control In",
            "posX": 588.4615600461799,
            "posY": 372.30770596385759,
            "uid": "14641672705827"
        },
        {
            "focused": false,
            "internalState": {
                "chanNumber": 4
            },
            "name": "Eos Channel",
            "posX": 441.53847773399639,
            "posY": 371.53847516641162,
            "uid": "14641672816604"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Control Eos Channel 4\nwith MIDI Control Change 3:"
            },
            "name": "Notes",
            "posX": 131.53846636326372,
            "posY": 351.53847443281592,
            "uid": "14641672911292"
        },
        {
            "focused": false,
            "internalState": {
                "channel": 1,
                "key": 53,
                "useDefaultChannel": true
            },
            "name": "MIDI Note In",
            "posX": 660.76925500610264,
            "posY": 509.23078790924319,
            "uid": "14641673571716"
        },
        {
            "focused": false,
            "internalState": {
                "maxValue": 1,
                "message": "/eos/sub/5=value",
                "minValue": 0,
                "useInteger": false
            },
            "name": "OSC Out",
            "posX": 438.46155454421245,
            "posY": 510.76924950413519,
            "uid": "14641673666447"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Control Submaster 5\nwith MIDI Note F4 velocity value:\n(using custom OSC message)"
            },
            "name": "Notes",
            "posX": 100.76923446542426,
            "posY": 488.46155637820158,
            "uid": "14641674643538"
        },
        {
            "focused": false,
            "internalState": {
                "keyName": "go"
            },
            "name": "Eos Key",
            "posX": 443.84617012633436,
            "posY": 872.30772430374896,
            "uid": "14641676048667"
        },
        {
            "focused": false,
            "internalState": {
                "threshold": 0.5
            },
            "name": "Threshold",
            "posX": 577.69232888193608,
            "posY": 845.38464639313941,
            "uid": "14641676111894"
        },
        {
            "focused": false,
            "internalState": {
                "value": 0.86205876854865848
            },
            "name": "Random Value",
            "posX": 713.84618002987565,
            "posY": 892.30772503734465,
            "uid": "14641676185322"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Press Go Button when \nrandom value is above 50%:"
            },
            "name": "Notes",
            "posX": 119.23077360412795,
            "posY": 901.53849460669642,
            "uid": "14641676487572"
        },
        {
            "focused": false,
            "internalState": {
                "keyName": "enter"
            },
            "name": "Eos Key",
            "posX": 454.61540129057818,
            "posY": 641.53848506995291,
            "uid": "14641677093014"
        },
        {
            "focused": false,
            "internalState": {
                "channel": 1,
                "key": 36,
                "useDefaultChannel": true
            },
            "name": "MIDI Note In",
            "posX": 609.23079157722145,
            "posY": 651.53848543675076,
            "uid": "14641677322890"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Map MIDI Note C3\nto Eos Enter key:"
            },
            "name": "Notes",
            "posX": 198.46154574106458,
            "posY": 646.92310065207482,
            "uid": "14641677533568"
        },
        {
            "focused": false,
            "internalState": {
                "text": "(see 2. tab in drawer for Key configuration)"
            },
            "name": "Notes",
            "posX": 336.92308928134219,
            "posY": 750.76925830728305,
            "uid": "14641678792306"
        }
    ],
    "connections": [
        "14641671729500|0->14641672126281|0",
        "14641671733478|0->14641672117464|0",
        "14641672705827|0->14641672816604|0",
        "14641673571716|0->14641673666447|0",
        "14641676111894|0->14641676048667|0",
        "14641676185322|0->14641676111894|1",
        "14641677322890|0->14641677093014|0"
    ],
    "planeX": -43,
    "planeY": -109,
    "version": 0.10000000000000001
}

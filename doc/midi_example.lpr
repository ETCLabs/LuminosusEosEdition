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
            "posX": 639,
            "posY": 140,
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
            "posX": 642,
            "posY": 226,
            "uid": "14641671733478"
        },
        {
            "focused": false,
            "internalState": {
                "cueList": 1,
                "cueNumber": 2
            },
            "name": "Eos Cue",
            "posX": 432,
            "posY": 226,
            "uid": "14641672117464"
        },
        {
            "focused": false,
            "internalState": {
                "cueList": 1,
                "cueNumber": 1
            },
            "name": "Eos Cue",
            "posX": 432,
            "posY": 140,
            "uid": "14641672126281"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Map MIDI program 1 and 2 \nto Cue 1 and 2 in Cue List 1:"
            },
            "name": "Notes",
            "posX": 129,
            "posY": 177,
            "uid": "14641672301491"
        },
        {
            "focused": false,
            "internalState": {
                "channel": 1,
                "target": 3,
                "useDefaultChannel": true
            },
            "name": "MIDI Control In",
            "posX": 589,
            "posY": 373,
            "uid": "14641672705827"
        },
        {
            "focused": false,
            "internalState": {
                "chanNumber": 4
            },
            "name": "Eos Channel",
            "posX": 442,
            "posY": 372,
            "uid": "14641672816604"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Control Eos Channel 3\nwith MIDI Control Change 4:"
            },
            "name": "Notes",
            "posX": 132,
            "posY": 352,
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
            "posX": 661,
            "posY": 510,
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
            "posX": 439,
            "posY": 511,
            "uid": "14641673666447"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Control Submaster 5\nwith MIDI Note F4 velocity value:\n(using custom OSC message)"
            },
            "name": "Notes",
            "posX": 101,
            "posY": 489,
            "uid": "14641674643538"
        },
        {
            "focused": false,
            "internalState": {
                "keyName": "go"
            },
            "name": "Eos Key",
            "posX": 444,
            "posY": 873,
            "uid": "14641676048667"
        },
        {
            "focused": false,
            "internalState": {
                "threshold": 0.5
            },
            "name": "Threshold",
            "posX": 578,
            "posY": 846,
            "uid": "14641676111894"
        },
        {
            "focused": false,
            "internalState": {
                "value": 0.63651247483120621
            },
            "name": "Random Value",
            "posX": 714,
            "posY": 893,
            "uid": "14641676185322"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Press Go Button when \nrandom value is above 50%:"
            },
            "name": "Notes",
            "posX": 120,
            "posY": 902,
            "uid": "14641676487572"
        },
        {
            "focused": true,
            "internalState": {
                "keyName": "enter"
            },
            "name": "Eos Key",
            "posX": 455,
            "posY": 642,
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
            "posX": 610,
            "posY": 652,
            "uid": "14641677322890"
        },
        {
            "focused": false,
            "internalState": {
                "text": "Map MIDI Note C3\nto Eos Enter key:"
            },
            "name": "Notes",
            "posX": 199,
            "posY": 647,
            "uid": "14641677533568"
        },
        {
            "focused": false,
            "internalState": {
                "text": "(see 2. tab in drawer for Key configuration)"
            },
            "name": "Notes",
            "posX": 337,
            "posY": 751,
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
    "planeX": -70,
    "planeY": -108,
    "version": 0.10000000000000001
}

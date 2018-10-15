
var projects = [
            { img: "tutorial", text: "Before we start: you can <b>hide and show</b> this tutorial at anytime to <b>try out</b> \
the described features!" },
            { img: "projects", text: "Before adding blocks, you should create a <b>new project</b> \
in the third tab on the right side menu.<br>\
The project is <b>saved automatically</b> every few seconds!" },
            { img: "gui-settings", text: "The projects <b>contains all blocks</b> while settings like \
'Click Sounds', 'Snap to Grid' and <b>OSC connections are stored globally</b>." },
            { img: "import", text: "You can <b>export</b> projects as <b>template files</b> and <b>import</b> them by \
<b>drag'n'dropping</b> them to the application window." },
        ]

var blocks = [
            { img: "block-list", text: "You can <b>add</b> a block by clicking on the name of the block in the menu on the right side." },
            { img: "block-search", text: "Pro Tips: Use the <b>search</b> to quickly find a block!<br>\
Select an in- or output before adding a block to directly connect it." },
            { img: "block-buttons", text: "After <b>selecting</b> a block by clicking on it you can <b>delete</b> it \
with the delete key or by clicking on the trash can in the bottom left. You can also <b>duplicate</b> a block with those buttons." },
            { img: "help-text", text: "Find out how to use a block by looking in its <b>help text</b>!<br>\
It often contains useful information and usage examples." },
            { img: "block-settings", text: "You can further adjust a block in its \
<b>block settings</b> after selecting it - either below the block or in the second tab in the right side menu." },
            { img: "suggestions", text: "Start to <b>connect</b> an output with an input by clicking on one of them.<br>\
You can - but don't have to - use the appearing <b>suggestions</b>. Then ..." },
            { img: "flow", text: "... click on the other in- or output to create the connection. Double click on an in- or output to remove the connections.<br>\
The 'data flow' is from <b>left to right</b>." },
            { img: "node-types", text: "<b>Round</b> in- and outputs are for <b>0-100%</b> values and <b>rectangular</b> ones are for <b>time</b> values.<br>\
Note: Some value modification blocks can handle both types." },
            { img: "multiple", text: "An input can be connected to <b>multiple</b> outputs and vice-versa.<br>\
Long click on an input to change the <b>merge behavior</b> from LTP to HTP." },
            { img: "inspector", text: "Anytime you are unsure what is happening, use an <b>Inspector Block</b> to see what values are being sent!" },
            { img: "add-to-group", text: "Use <b>groups</b> to structure a project. <b>Add a block</b> to the group by selecting it and \
clicking on 'Focused Block' in the Group Block." },
            { img: "in-group", text: "See the blocks in a group by clicking on <b>'Open'</b>.<br>\
<b>Leave</b> a group by clicking on the arrow next to the title at the top of the screen." },
            { img: "combinations", text: "You can save <b>combinations of blocks</b> you want to reuse.<br>\
Tip: Start in a new <b>empty group</b> to not save unwanted blocks!" },
        ]

var lighting = [
            { img: "lighting", text: "To control an <b>Eos</b> or <b>Hog</b> console, click on \
<b>'Connect a Console'</b> on the welcome screen and follow the instructions. The connection will appear \
in the settings under 'Lighting Console'." },
            { img: "eos-fader-bank", text: "You can then use the Eos or Hog Blocks. For example the <b>Fader Bank Block</b> \
that can also be mapped to a MIDI controller." },
            { img: "eos-cue-list", text: "One or more <b>Cue List Blocks</b> can be added to show different cue lists. \
The currently active cue is highlighted and cues can be moved, renamed, deleted and notes can be added." },
            { img: "virtual-fixture-projector", text: "To easily control Luminosus functions from \
the console, you can use the <b>sACN In</b> blocks. In this example a projector is controllable as \
a virtual fixture." },
        ]

var osc = [
            { img: "custom-osc", text: "<b>Custom OSC</b> blocks can be used to send and receive arbitrary OSC commands." },
            { img: "custom-osc-settings", text: "They use the <b>'Custom OSC' connection</b>. This means \
to send custom OSC commands to a lighting console, you have to \
create <b>another connection</b> to it with Custom OSC!" },
            { img: "osc-in", text: "To <b>receive</b> OSC messages, enter the path <b>without arguments</b> in the OSC In Block.<br>\
Either the value of the first argument or an impulse on each incoming message is sent to the output." },
            { img: "osc-out", text: "To <b>send</b> custom OSC commands, enter the path followed by the arguments.<br>\
'&lt;value&gt;' is replaced with the incoming value. The min and max values can be set in the block settings." },
        ]

var midi = [
            { img: "midi-settings", text: "To use MIDI Mapping or custom MIDI messages, make sure your MIDI controller \
is <b>listed in the settings</b>. If you connected the controller after starting Luminosus, click on 'Refresh'." },
            { img: "midi-mapping", text: "Almost every <b>fader and button</b> in the user interface can be <b>mapped to a MIDI controller</b>. \
To do so, follow the instructions in the 'MIDI Mapping' settings." },
            { img: "midi", text: "For more <b>complex tasks</b> the MIDI In and Out blocks can be used.<br>\
MIDI Show Control is not supported." },
            { img: "midi-learn", text: "Click on the <b>'Learn' button</b> in the block settings and \
push the MIDI controller button or fader to <b>configure the MIDI In</b> blocks." },
        ]

var s2l = [
            { img: "s2l-blocks", text: "To create a Sound2Light setup, you usually start with a <b>Spectrum Block</b>. \
If you don't want to restrict the input to a frequency range you can also use the <b>Volume Block</b>." },
            { img: "spectrum-source", text: "After adding the Spectrum Block, set the correct <b>audio input device</b> in its settings.<br>\
You can use <b>multiple</b> Spectrum Blocks with different audio inputs." },
            { img: "spectrum-inspector", text: "Normally, the block outputs the level of the <b>loudest frequency</b> in the specified range." },
            { img: "frequency-mode", text: "By activating the 'Frequency Mode', the block outputs the <b>pitch</b> of \
the highest frequency that reaches the top of the frequency range." },
            { img: "frequency-range", text: "The frequency range can be <b>moved around</b> and the width can be changed in the \
settings or with two fingers. The height marks the 100% level." },
            { img: "spectrogram", text: "The <b>spectrogram</b> view is very useful to see which frequency has a periodic beat.<br>\
The block itself can also be <b>resized</b>." },
            { img: "spectrum-channel", text: "Finally you can connect the Spectrum Block directly to an \
Eos Channel, Group or Submaster to <b>control its level</b>." },
            { img: "multiply", text: "Controlling the intensity can be turned <b>on and off</b> by <b>multiplying</b> the \
signal with 0 or 1. A Toggle Block with Cue or OSC inputs could be used for that." },
            { img: "smooth", text: "In many cases using the volume directly leads to too <b>flickery</b> results. \
A <b>Smooth Block</b> can be used to change that. An <b>Inspector Block</b> is very useful to see the result." },
            { img: "threshold", text: "If you want to <b>switch a light</b> on and off instead of controlling its intensity \
continuously, use a <b>Threshold Block</b>." },
            { img: "delay", text: "By using a <b>Delay Block</b> you can trigger something only in the case that \
the level is above a certain threshold for a <b>specified time</b>." },
            { img: "drop-detection", text: "Use the Delay, Decay, Multiply, Hold Max and Scale / Invert blocks \
to create <b>complex effects</b> like a music 'drop' detection." },
            { img: "synthetic-beat", text: "Instead of using a Threshold Block, a perfect <b>synthetic beat</b> is sometimes better. \
You can create it with a BPM Tap Block and a Tick Generator." },
            { img: "beat-detection", text: "A BPM Detection Block can be used instead of the tap block to <b>detect \
the BPM</b> automatically. Remember to set the correct audio source." },
            { img: "bpm-switch", text: "Both BPM blocks can be combined as shown above to be able to <b>switch \
between automatic and tapped BPM</b>." },
            { img: "eos-effect-speed", text: "To control the <b>speed of an Eos Effect</b>, connect a BPM Tap or BPM Detection \
Block to it." },
            { img: "eos-speed-master", text: "In the setup shown above, the speed of <b>multiple Eos effects</b> can be \
controlled either by <b>tapped or detected BPM</b>." },
        ]

var media = [
            { img: "cue-music", text: "You can start the <b>playback of a song</b> exactly when a specific cue was activated \
by connecting an Eos Cue Block with an Audio Playback Block." },
            { img: "pling", text: "Map a <b>MIDI button</b> to the play button to play a <b>sound effect</b> when it is pressed. \
See the block settings to change the playback behavior." },
            { img: "cue-after-sound", text: "It is also possible to activate a cue or effect at the <b>begin or end</b> of a sound." },
            { img: "presentation-control", text: "Luminosus can control <b>PowerPoint</b> presentations when they \
are running on the <b>same PC in foreground</b> by emulating keyboard presses. (OpenOffice etc. works, too)" },
            { img: "single-slide", text: "You can trigger a <b>specific slide</b> with the \
PowerPoint Slide Block, for example when a cue is activated." },
            { img: "dmx-slide", text: "To easily <b>map cues to slides</b>, connect an 'sACN In Block' to the \
'DMX -> Slide Number Block' and set the desired slide number in the cue as the DMX value of this channel." },
            { img: "key-press", text: "Additionally you can emulate any <b>keyboard button</b> press with the \
Key Press Block, i.e. to control <b>other media players</b> like VLC." },
            { img: "projector", text: "<b>Projectors</b> that support the PJLink network protocol can also be \
controlled. This is useful to close the <b>shutter</b> after a video was played to minimize light scatter." },
        ]

var effects = [
            { img: "daisy-chain", text: "You can <b>daisy-chain</b> multiple Eos Channel Blocks to \
create effects like a 'volume meter', where the loudness controls how many lights are on." },
            { img: "knight-rider", text: "By using blocks from the 'Line / Matrix' category, it is possible \
to build a symmetric <b>'Knight Rider'-like</b> effect by mirroring one half of the row." },
            { img: "join-rows", text: "To create <b>two-dimensional effects</b>, join multiple rows with the 'Join Rows Block'. \
The <b>limiting factor</b> is the OSC connection, therefore it is not recommended to control more than 20 Channels." },
            { img: "diagonal", text: "<b>Diagonal</b> effects are also possible by using the <b>Mirror / Rotate Block</b>." },
            { img: "color-random", text: "Experiment with the Generator, Gradient and Hue + Sat + Intensity \
Blocks to create <b>colorful or random effects</b>." },
            { img: "fire-effect", text: "A random flickering <b>fire effect</b> for example could look like shown above." },
        ]

var tutorials = [
            { category: "Basics", title: "Create & Save Projects", slides: projects, folder: "basics" },
            { category: "Basics", title: "Blocks & Connections", slides: blocks, folder: "basics" },
            { category: "Connect", title: "Lighting Console", slides: lighting, folder: "connect" },
            { category: "Connect", title: "Custom OSC", slides: osc, folder: "connect" },
            { category: "Connect", title: "MIDI", slides: midi, folder: "connect" },
            { category: "Use Case", title: "Sound2Light", slides: s2l, folder: "s2l" },
            { category: "Use Case", title: "Media & Presentations", slides: media, folder: "media" },
            { category: "Use Case", title: "Advanced Effects", slides: effects, folder: "effects" },
        ]

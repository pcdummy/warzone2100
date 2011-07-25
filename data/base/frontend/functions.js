.pragma library
var backgrounds = ["Background 0",
                   "Background 1",
                   "Background 2",
                   "Background 3",
                   "Background 4",
                   "Background 5",
                   "Background 6"]

function getRandBackground()
{
    return backgrounds[(Math.floor(Math.random() * backgrounds.length))];
}


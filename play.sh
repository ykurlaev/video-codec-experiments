#!/bin/sh
mplayer -demuxer rawvideo -rawvideo w=720:h=486:y8 "$@"

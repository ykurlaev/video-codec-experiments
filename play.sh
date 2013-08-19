#!/bin/sh
mplayer -demuxer rawvideo -rawvideo w=640:h=346:y8 "$@"

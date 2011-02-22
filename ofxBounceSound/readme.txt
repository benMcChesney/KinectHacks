TuioKinect tracks simple hand gestures using the Kinect controller and sends control data based on the TUIO protocol. This allows the rapid creation of gesture enabled applications with any platform or environment that supports TUIO. Check out this extensive list of TUIO enabled software for further information: http://www.tuio.org/software

Note: This is still a preliminary proof of concept implementation, which still needs several improvements to become fully usable. Nevertheless it should work out of the box with most TUIO client applications.

Configuration: You can adjust the near/far threshold using the -/+ and </> keys.

Todo:
# improve calibration by defining an active plane/region in 3D space rather than using two simple distance thresholds
# color/depth calibration to enable more detailed segmentation of the hands
# open/closed hand detection in order to allow dedicated tracking modes.
# create a windows project and binaries

Acknowledgments: This application has been created by Martin Kaltenbrunner at the Interface Culture Lab. It is based on OpenFrameworks and its ofxKinect example project, which is making use of libfreenect by the OpenKinect project. Many thanks to all the developers involved in the process, without their work this application wouldn't have been that easy to implement.

In order to compile this application you will need a working installation of OpenFrameworks and the ofxKinect extension. The provided binary has been compiled and tested under Mac Os 10.6 on an Intel machine only.

© 2010 by Martin Kaltenbrunner
http://modin.yuri.at/


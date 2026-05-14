#!/bin/bash

root -q -b 'DrawTrianglePlot.C+("/opt/ppd/t2k/users/barrowd/Fits/Contours/Fit6/Contours_wRC_UnSmeared/RootFiles/Contours_1D_wRC_UnSmeared_CredibleInterval.root","/opt/ppd/t2k/users/barrowd/Fits/Contours/Fit6/Contours_wRC_UnSmeared/RootFiles/Contours_2D_wRC_UnSmeared_CredibleInterval.root",true)'
root -q -b 'DrawTrianglePlot.C+("/opt/ppd/t2k/users/barrowd/Fits/Contours/Fit6/Contours_woRC_UnSmeared/RootFiles/Contours_1D_woRC_UnSmeared_CredibleInterval.root","/opt/ppd/t2k/users/barrowd/Fits/Contours/Fit6/Contours_woRC_UnSmeared/RootFiles/Contours_2D_woRC_UnSmeared_CredibleInterval.root",false)'

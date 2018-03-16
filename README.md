# TTLViewer
TTL Viewer plugin for the open-ephys [plugin-GUI](https://github.com/open-ephys/plugin-GUI/ "pluguin-GUI"). This is a modification of the standard SpikeViewer plugin that it is adaptated for the TTLDetector.
## What is new:
* Each plot occupies the width of the window and the electrodes are shown in a single column. You can display 32 plots at the same time. If you click the left mouse button two times on the plot, you can extend that plot to full screen.
* Exist an option to fix the number of lines to display at the same time in the same plot. This number must be between 1 and 5.
* Also, exist another option to change the voltage (in milliVolts) of every plot.  

Both in the option of the lines and in the voltage it can be changed values while the record is running.

## Installation
Copy the TTLViewer folder to the plugin folder of your GUI. Then build the plugin as described in the [wiki](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/491544/Installation "wiki").

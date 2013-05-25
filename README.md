DroughtMonitor
==============

An experiment in using an Arduino and sensors to create personal drought monitors to crowdsource large scale weather information. See the Fritzing diagram and the code comments for more information.

Parts
-----

The Fritzing file will show the wiring diagram, based on the Arduino Prototype board we're using. Future updates will include a breadboarded version for more clarity.

* Arduino Uno
* A Sparkfun WiFly Shield
* An Arduino Prototype Shield
* A DHT22 temperature sensor
* Two galvanized carriage bolts (depicted as antennae on the Fritzing diagram)
* An account at Xively (formerly Cosm and Pachube)
* A wireless network to connect to.


Deploy notes
------------

The housing we chose is a $8 solar powered sidewalk light from target. It's big enough to fit the Arduino+2 boards and the battery pack, it gets the package up off the ground and soon we'll wire the solar power into the battery pack for some additional juice. The first iteration does not have that. This housing kept everything nice and dry during a driving spring thunderstorm in Nebraska.

As configured, we've got it running on four AA batteries. They lasted 14 hours before becoming too weak to drive a wifi signal through an exterior wall plus another 5 feet. Replaced the batteries and it's back up and running. You can see the live data feed [here](https://xively.com/feeds/128717/workbench). Power is obviously going to be an issue -- no one wants to replace batteries every 12 hours, even if they are rechargeable. It's not convenient. An issue on Github has been created for this.

As the first deploy progresses, we'll document our housing and issues we'll find.
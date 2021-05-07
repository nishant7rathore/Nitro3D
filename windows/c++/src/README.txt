/**
* Nishant Rathore 
* Ronak Parmar
* 

Our main goal was focusing on economy and expansion

Things we were able to successfully implement and that works most of time consistently 


1) Build Order
2) Training units 
3) Intelligent building placement 
4) Finding building locations
5) Resource locations are stored in a distance map (MapTools.cpp)
6) BaseLocations are computed
7) Cannons and pylons location are computed for each base
8) Worker Management - this ensures that workers are assigned on minerals and refinery equally 
 (20 for first base mineral field and 10 for each additional base)
  ( 2-3 per refinery)
9) Units are sent to attack the enemy base
10) Smart Scouting is implemented
11) Enemy and own units Modelling are designed
12) Units are part of a base (BaseManager)
13) N arm bandit algo implemented so that bot can learn from its experience

Things we tried but didn't get working:

1) Combat Simulator inital code implemented (incomplete)
2) Attack seems to be weak
3) Bot doesn't win against the built-in; only wins rarely



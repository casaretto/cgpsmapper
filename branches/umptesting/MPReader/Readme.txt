This project was my attempt to re-write whole cgpsmapper - the idea was -

 - it uses GUI - very simple one (this was one of the main complain I hear most often) - however still will be portable
 - it uses SQLite - to resolve problem of insufficient memory (by option base can be created in-memory or on HDD - of course I know that it will decrease performance A LOT)
 - it uses LUAscript - so internal processing / scripts could be used

So far I did not write too much - 
 - I did start to write new MP reader (not finished!)
 - LUAscript has been integrated
 - data are read into DB
 - Maproute has been integrated (available with scripts as well)

Using simple command window it is possible to query data already as well as start internal maproute.

In order to compile it following components are required:

	WX (available through PATH)
	sqlite ( ..\sqlite-source )
	boost ( optionally - can be disable - but only by changes in the code )
	LuaPlus51 ( through PATH)
	shapelib (with sources already)
	
It means - at the moment whole processing for creation of IMG file is missing, so I don't know if these sources are interesting to be used as base for the new attempt... 
Anyway - I'm publishing that with hope, it will be useful... as next step to create new version of the program ;)
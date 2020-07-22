'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:24:31 pm'!SequenceableCollection removeSelector: #size!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:22:50 pm'!!Interval reorganize!('comparing' = hash hashMappedBy:)('accessing' at: at:put: first increment last size)('adding' add:)('removing' remove:)('copying' copy)('testing' includes:)('enumerating' collect: do: reverseDo:)('printing' printOn: storeOn:)('private' setFrom:to:by: species)!!Interval methodsFor: 'accessing'!

size	(self isKindOf: Number) ifFalse: [^super size].	step < 0		ifTrue: [start < stop				ifTrue: [^0]				ifFalse: [^stop - start // step + 1]]		ifFalse: [stop < start				ifTrue: [^0]				ifFalse: [^stop - start // step + 1]]! !!Interval methodsFor: 'testing'!

includes: aNumber	"Answer whether aNumber is one of the receiver's elements.	 Re-implemented here to gain some performance."	(aNumber isKindOf: Number) ifFalse: [^super includes: aNumber].	(aNumber < start or: [aNumber > stop]) ifTrue: [^false].	^(aNumber - start) isMultipleOf: step! !'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:25:00 pm'!!Number methodsFor: 'testing'!

isMultipleOf: aNumber 	"Answers true if the receiver is an exact multiple of 	 aNumber, otherwise false."	^self \\ aNumber = 0!

notMultipleOf: aNumber 	"Answers false if the receiver is an exact multiple of 	 aNumber, otherwise true."	^(self isMultipleOf: aNumber) not! !'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:32:40 pm'!!Point methodsFor: 'truncation and round off'!

truncated	"Answer a new Point that is the receiver's x and y truncated."	^x truncated @ y truncated! !!Point methodsFor: 'point functions'!

nearestTo45DegreeLineThrough: refPoint onGrid: aGridPoint	"Answers the closest integer point to the receiver which	 is both on the grid given by aGridPoint and on a	 45 degree line through refPoint."	| thePoints currentDistance shortestDistance closestPoint |	" Generate the four nearest points on lines through refPoint. "	thePoints _ OrderedCollection new.	(Array with: 1@0 with: 1@1 with: 0@1 with: -1@1) do: [		:aSlope | thePoints add: (self pointNearestLine: refPoint to: refPoint + aSlope)].	" Generate the four distances from the nearest points on the line to aPoint,		find the shortest one, and thus the closest point. "	shortestDistance _ 1000000.	thePoints do:		[:eachPoint | currentDistance _ self dist: eachPoint.				(currentDistance <= shortestDistance ifTrue: [closestPoint _ eachPoint.														shortestDistance _ currentDistance])		].	^closestPoint grid: aGridPoint!

pointNearestLine: point1 to: point2	"Answers the closest integer point to the receiver on the	 line determined by (point1, point2)."	| relPoint delta |	delta _ point2 - point1. 			"normalize coordinates"	relPoint _ self - point1.	delta x = 0 ifTrue: [^point1 x@y].	delta y = 0 ifTrue: [^x@point1 y].	delta x abs > delta y abs 		"line more horizontal?"		ifTrue: [^x@(point1 y + (x * delta y // delta x))]		ifFalse: [^(point1 x + (relPoint y * delta x // delta y))@y]	"43@55 pointNearestLine: 10@10 to: 100@200"! !Point removeSelector: #truncatedGrid:!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:33:01 pm'!!Rectangle methodsFor: 'truncation and round off'!

truncated	"Answer a Rectangle whose origin and corner are truncated."	^Rectangle origin: origin truncated corner: corner truncated! !'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:34:24 pm'!Point subclass: #PcbObject	instanceVariableNames: ''	classVariableNames: ''	poolDictionaries: ''	category: 'Printed-Circuits'!PcbObject comment:'I am the abstract superclass of objects which appear on a printed circuitboard.  Instance variables x and y (inherited from Point) indicate thelocation of my instances.'!!PcbObject methodsFor: 'accessing'!

refPoint	"Answer with my reference point."	^self x@self y!

refX	"Answer with my reference x value."	^self x!

refY	"Answer with my reference y value."	^self y!

size	"Answer with a value representing my size (for sorting purposes)."	self subclassResponsibility! !!PcbObject methodsFor: 'file handling'!

writeBottomDrawerOn: aFileStream	"Write the closest possible representation of the receiver	 in bottom drawer format on aFileStream."	aFileStream nextPutAll: '/ '.	aFileStream nextPutAll: self asBottomDrawer.	aFileStream nextPutAll: ' //'.	aFileStream lf! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!PcbObject class	instanceVariableNames: ''!!PcbObject class methodsFor: 'private'!

getIntegerFrom: aFileStream 	"Gets a sequence of 1 or more digit characters from 	 aFileStream, terminated by any non-digit. Answer the 	 corresponding integer."	| aChar anInteger |	anInteger _ 0.	aChar _ aFileStream next.	[aChar isDigit] whileFalse: [aChar _ aFileStream next].	[aChar isDigit] whileTrue: [			anInteger _ anInteger * 10 + aChar digitValue.			aChar _ aFileStream next].	^anInteger!

getSlashFrom: aFileStream	"Gets a Slash character from aFileStream."	| aChar |	aChar _ aFileStream next.	[aChar = $/] whileFalse: [aChar _ aFileStream next].	^aChar! !'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:34:48 pm'!PcbObject subclass: #Pad	instanceVariableNames: 'diameter '	classVariableNames: 'DefaultDiameter PadSlide '	poolDictionaries: ''	category: 'Printed-Circuits'!Pad comment:'I represent a Pad (component fixing) on a printed circuit board.I add an instance variable diameter, indicating my size.'!!Pad methodsFor: 'accessing'!

diameter	"Answer with the diameter of the receiver."	^diameter!

diameter: aNumber	"Set the diameter of the receiver."	diameter _ aNumber!

position	"Answers a point equal to current position of the receiver."	^self x@self y!

size	"Answer with my diameter"	^diameter! !!Pad methodsFor: 'testing'!

isZeroDiameter	"Answer whether the receiver has a diameter of zero."	^self diameter = 0! !!Pad methodsFor: 'comparing'!

= anObject	"Answer whether the receiver is the same as anObject."	^super = anObject and: [self diameter = anObject diameter]! !!Pad methodsFor: 'truncation and rounding'!

rounded	"Answer with a new Pad that is a rounded version of the receiver."	^Pad point: super rounded diameter: (self diameter rounded max: 1)!

truncated	"Answer with a new Pad that is a truncated version of the receiver."	^Pad		x: self x rounded		y: self y rounded		diameter: (self diameter truncated max: 1)! !!Pad methodsFor: 'transforming'!

scaleBy: aPoint 	"Answer a new Pad scaled by aPoint"	^Pad		point: (super scaleBy: aPoint)		diameter: self diameter * (aPoint x min: aPoint y)!

translateBy: delta 	"Answer a new Pad translated by delta."	^Pad		point: (super translateBy: delta)		diameter: self diameter! !!Pad methodsFor: 'point functions'!

grid: aPoint	"Answer with a new Pad, with the endpoints rounded	 to a grid given by aPoint."	^Pad point: (super grid: aPoint) diameter: self diameter! !!Pad methodsFor: 'printing'!

printOn: aStream 	"The receiver prints on aStream in terms of infix notation."	x printOn: aStream.	aStream nextPut: $@.	y printOn: aStream.	aStream nextPutAll: ' dia '.	diameter printOn: aStream.! !!Pad methodsFor: 'file handling'!

asBottomDrawer	"Answers with a string which is the closest Bottom Drawer 	 representation of the receiver."	^(self bottomDrawerPadSize: self diameter), ' X' ,		self x printString, ' Y', self y printString! !!Pad methodsFor: 'private'!

bottomDrawerPadSize: aDiameter 	"Answers a String with the nearest Bottom Drawer Pad size to 	aDiameter. "	PadSlide associationsDo: [:assoc |		(assoc value includes: aDiameter) ifTrue: [^assoc key asString]].	^self error: 'Diameter outside range of Pad sizes for the current slide.'!

setX: xPad setY: yPad setDiameter: diameterPad	"Set up the x, y and diameter for the receiver." 	x _ xPad.	y _ yPad.	diameter _ diameterPad! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!Pad class	instanceVariableNames: ''!!Pad class methodsFor: 'instance creation'!

bottomDrawerPadFrom: aFileStream 	"Gets a BottomDrawer Pad representation from aFileStream.  	 Answers the corresponding Pad."	| anInterval size aPad |	anInterval _ PadSlide at: ('P', (self getIntegerFrom: aFileStream) printString) asSymbol.	aPad _ self				x: (self getIntegerFrom: aFileStream)				y: (self getIntegerFrom: aFileStream)				diameter: anInterval first + anInterval last // 2.	self getSlashFrom: aFileStream.	self getSlashFrom: aFileStream.		" Discard double slash"	aFileStream next.					" Discard lf"	^aPad!

point: aPoint 	"Answer a new instance of me with coordinates given 	 by the point aPoint, and diameter equal to the DefaultDiameter."	^self		point: aPoint		diameter: DefaultDiameter!

point: aPoint diameter: aNumber	"Answer a new instance of me with coordinates given 	 by the point aPoint, and diameter equal to aNumber."	^self		x: aPoint x		y: aPoint y		diameter: aNumber!

x: xInteger y: yInteger 	"Answer a new instance of me with coordinates xInteger 	 and yInteger, and diameter equal to the DefaultDiameter."	^self		x: xInteger		y: yInteger		diameter: DefaultDiameter!

x: xInteger y: yInteger diameter: diameterInteger 	"Answer a new instance of me with coordinates xInteger 	 and yInteger, diameter diameterInteger."	^self new		setX: xInteger		setY: yInteger		setDiameter: diameterInteger! !!Pad class methodsFor: 'class initialization'!

initialize.	"Initialize class variables."	"Pad initialize."	DefaultDiameter _ 60.   "Default Pad diameter"	PadSlide _ Dictionary new.		" EMMA Pad Slide"	PadSlide at: #P1 put: (Interval from: 0 to: 7).			" Nominal 5 thou "	PadSlide at: #P2 put: (Interval from: 8 to: 12).			" Nominal 10 thou "	PadSlide at: #P3 put: (Interval from: 13 to: 17).		" Nominal 15 thou "	PadSlide at: #P4 put: (Interval from: 18 to: 22).		" Nominal 20 thou "	PadSlide at: #P5 put: (Interval from: 23 to: 29).		" Nominal 25 thou "	PadSlide at: #P6 put: (Interval from: 30 to: 35).		" Nominal 32 thou "	PadSlide at: #P7 put: (Interval from: 36 to: 45).		" Nominal 37 thou "	PadSlide at: #P8 put: (Interval from: 46 to: 55).		" Nominal 50 thou "	PadSlide at: #P9 put: (Interval from: 56 to: 68).		" Nominal 60 thou "	PadSlide at: #P10 put: (Interval from: 69 to: 77).		" Nominal 75 thou "	PadSlide at: #P11 put: (Interval from: 78 to: 90).		" Nominal 80 thou "	PadSlide at: #P12 put: (Interval from: 91 to: 112).		" Nominal 100 thou "	PadSlide at: #P13 put: (Interval from: 113 to: 137).	" Nominal 125 thou "	PadSlide at: #P14 put: (Interval from: 138 to: 175).	" Nominal 150 thou "	PadSlide at: #P15	put: (Interval from: 176 to: 10000). "Nominal 200 thou "! !!Pad class methodsFor: 'class access'!

defaultDiameter	"Answer with the default pad size."	^DefaultDiameter! !Pad initialize!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:35:14 pm'!PcbObject subclass: #TrackSegment	instanceVariableNames: 'endX endY width '	classVariableNames: 'DefaultWidth TrackSlide '	poolDictionaries: ''	category: 'Printed-Circuits'!TrackSegment comment:'I represent a segment of a track on a printed circuit board.  I addinstance variables indicating my end point (endX, endY) and mysize (width).'!!TrackSegment methodsFor: 'accessing'!

endPoint	"Answers a point corresponding to the end of the receiver."	^self x2 @ self y2!

size	"Answer with my width."	^width!

startPoint	"Answers a point corresponding to the start of the receiver."	^self x1 @ self y1!

width	"Answer with the track width."	^width!

x1	"Answer with the starting x coordinate."	^x!

x2	"Answer with the end x coordinate."	^endX!

y1	"Answer with the starting y coordinate."	^y!

y2	"Answer with the end y coordinate."	^endY! !!TrackSegment methodsFor: 'testing'!

isZeroLength	"Answer whether the receiver is of zero Length."	^self startPoint = self endPoint! !!TrackSegment methodsFor: 'comparing'!

= aTrackSegment 	"Answer whether the receiver is equal to aTrackSegment."	self species = aTrackSegment species		ifTrue: [^(self sameWidthAs: aTrackSegment)				and: [self sameLineAs: aTrackSegment]]		ifFalse: [^false]!

sameEndAs: aTrackSegment	"Answer whether the reciver has the same end value	 as aTrackSegment."	^self x2 = aTrackSegment x2 and: [self y2 = aTrackSegment y2]!

sameLineAs: aTrackSegment	"Answer whether the reciver has the same start and end points as	 aTrackSegment."	^(self sameStartAs: aTrackSegment) and: [self sameEndAs: aTrackSegment]!

sameStartAs: aTrackSegment	"Answer whether the reciver has the same starting value	 as aTrackSegment."	^self x1 = aTrackSegment x1 and: [self y1 = aTrackSegment y1]!

sameWidthAs: aTrackSegment	"Answer whether the receiver has the same width as aTrackSegment."	^self width = aTrackSegment width! !!TrackSegment methodsFor: 'truncation and rounding'!

rounded	"Answer with a new TrackSegment that is a rounded version 	 of the receiver."	^TrackSegment		from: self startPoint rounded		to: self endPoint rounded		width: (self width rounded max: 1)!

truncated	"Answer with a new TrackSegment that is a truncated version 	 of the receiver."	^TrackSegment		from: (self startPoint truncateTo: 1@1)		to: (self endPoint truncateTo: 1@1)		width: (self width truncated max: 1)! !!TrackSegment methodsFor: 'transforming'!

scaleBy: aPoint 	"Answer a new TrackSegment scaled by aPoint."	^TrackSegment		from: (self startPoint scaleBy: aPoint)		to: (self endPoint scaleBy: aPoint)		width: self width * (aPoint x min: aPoint y)!

translateBy: delta 	"Answer with a new TrackSegment translated by delta."	^TrackSegment		from: (self startPoint translateBy: delta)		to: (self endPoint translateBy: delta)		width: self width! !!TrackSegment methodsFor: 'point functions'!

grid: aPoint 	"Answer with a new TrackSegment, with the endpoints 	 rounded to a grid given by aPoint."	^TrackSegment		from: (self startPoint grid: aPoint)		to: (self endPoint grid: aPoint)		width: self width! !!TrackSegment methodsFor: 'printing'!

printOn: aStream 	"The receiver prints on aStream in terms of infix notation."	self x1 printOn: aStream.	aStream nextPut: $@.	self y1 printOn: aStream.	aStream nextPutAll: ' to '.	self x2 printOn: aStream.	aStream nextPut: $@.	self y2 printOn: aStream.	aStream nextPutAll: ' width '.	self width printOn: aStream.! !!TrackSegment methodsFor: 'file handling'!

asBottomDrawer	"Answers with a string which is the closest Bottom Drawer 	 representation to the receiver."	^'X', self x1 printString, ' Y', self y1 printString, ' ',		(self bottomDrawerTrackSize: self width),		' X', self x2 printString , ' Y', self y2 printString! !!TrackSegment methodsFor: 'private'!

bottomDrawerTrackSize: aTrackWidth 	"Answers a String with the nearest Bottom Drawer Track size to 	aTrackWidth. "	TrackSlide associationsDo: [:assoc |		(assoc value includes: aTrackWidth) ifTrue: [^assoc key asString]].	^self error: 'Diameter outside range of Track sizes for the current slide.'!

setX1: x1 setY1: y1 setX2: x2 setY2: y2 setWidth: trackWidth 	"If the first point is above and to the left of the second point, 	 set the instance variables as given.  Otherwise, reverse the 	 order of the points."	(x1 < x2 and: [y1 < y2])		ifTrue: [			x _ x1.  endX _ x2.			y _ y1.  endY _ y2]		ifFalse: [			x _ x2.  endX _ x1.			y _ y2.  endY _ y1].	width _ trackWidth! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!TrackSegment class	instanceVariableNames: ''!!TrackSegment class methodsFor: 'instance creation'!

bottomDrawerTrackFrom: aFileStream 	"Gets a Bottom Drawer Track representation from aFileStream 	 Answers the corresponding Track Segment"	| anInterval x1 y1 aTrackSegment |	x1 _ self getIntegerFrom: aFileStream.	y1 _ self getIntegerFrom: aFileStream.	anInterval _ TrackSlide at:		('L' , (self getIntegerFrom: aFileStream) printString) asSymbol.	aTrackSegment _ self				x1: x1				y1: y1				x2: (self getIntegerFrom: aFileStream)				y2: (self getIntegerFrom: aFileStream)				width: anInterval first + anInterval last // 2.	self getSlashFrom: aFileStream.	self getSlashFrom: aFileStream.		" Discard double slash"	aFileStream next.					" Discard lf"	^aTrackSegment!

from: point1 to: point2 	"Answer a new instance of me with endpoints given by point1 	 and point2, and width equal to the default width."	^self		x1: point1 x		y1: point1 y		x2: point2 x		y2: point2 y		width: DefaultWidth!

from: point1 to: point2 width: widthInteger 	"Answer a new instance of me with endpoints given by point1  	 and point2, and width equal to widthInteger."	^self		x1: point1 x		y1: point1 y		x2: point2 x		y2: point2 y		width: widthInteger!

x1: x1 y1: y1 x2: x2 y2: y2 	"Answer a new instance of me with endpoints (x1,y1), (x2,y2), with 	 the default width."	^self		x1: x1		y1: y1		x2: x2		y2: y2		width: DefaultWidth!

x1: x1 y1: y1 x2: x2 y2: y2 width: widthInteger 	"Answer a new instance of me with endpoints (x1,y1), (x2,y2),  	 with width given by widthInteger."	^self new		setX1: x1		setY1: y1		setX2: x2		setY2: y2		setWidth: widthInteger! !!TrackSegment class methodsFor: 'class initialization'!

initialize	"Initialize class variables."	"TrackSegment initialize."	DefaultWidth _ 20.   "Default Track Width"	TrackSlide _ Dictionary new.		"EMMA Track slide"	TrackSlide at: #L1 put: (Interval from: 0 to: 7).		" Nominal 5 thou "	TrackSlide at: #L2 put: (Interval from: 8 to: 15).		" Nominal 10 thou "	TrackSlide at: #L3 put: (Interval from: 16 to: 26).		" Nominal 20 thou "	TrackSlide at: #L4 put: (Interval from: 27 to: 34).		" Nominal 32 thou "	TrackSlide at: #L5 put: (Interval from: 35 to: 45).		" Nominal 37 thou "	TrackSlide at: #L6 put: (Interval from: 46 to: 55).		" Nominal 50 thou "	TrackSlide at: #L7 put: (Interval from: 56 to: 70).		" Nominal 60 thou "	TrackSlide at: #L8 put: (Interval from: 71 to: 90).		" Nominal 80 thou "	TrackSlide at: #L9 put: (Interval from: 15 to: 25).		" Nominal 20 thou "	TrackSlide at: #L12 put: (Interval from: 91 to: 125).	" Nominal 100 thou "	TrackSlide at: #L14 put: (Interval from: 126 to: 175).	" Nominal 150 thou "	TrackSlide at: #L15 put: (Interval from: 176 to: 225).	" Nominal 200 thou "	TrackSlide at: #L16 put: (Interval from: 226 to: 100000). " Anything larger!!"! !!TrackSegment class methodsFor: 'class access'!

defaultWidth	"Answer with the default track width."	^DefaultWidth! !TrackSegment initialize!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:35:31 pm'!Object subclass: #PrintedCircuit	instanceVariableNames: 'pads tracks grid '	classVariableNames: 'DefaultGrid '	poolDictionaries: ''	category: 'Printed-Circuits'!PrintedCircuit comment:'I represent a printed circuit board.  I have instance variables:tracks	A <Set> of the tracks making up this printed circuit board.pads	A <Set> of the pads making up this board.grid		A <Point> indicating the grid size of this printed circuit		board.  All Pads and TrackSegments are constrained to		lie on this grid.'!!PrintedCircuit methodsFor: 'initialize-release'!

initialize	"Initialize the pads and tracks of the receiver."	tracks _ Set new.	pads _ Set new.	grid _ DefaultGrid!

release	pads release.	tracks release.	grid release.	super release!

restart	"Discard the current contents of the receiver."	self initialize.	self changed: #all! !!PrintedCircuit methodsFor: 'accessing'!

grid	"Answer with the current grid (a Point)."	^grid!

grid: aPoint	"Set the current grid size."	grid _ aPoint!

pads	"Answer with the set of pads."	^pads!

tracks	"Answer with the set of tracks."	^tracks! !!PrintedCircuit methodsFor: 'adding'!

addPad: aPad	"Add aPad to the set of pads in the receiver, using the current	 grid size.  Answer with the pad just added"	^self addPad: aPad onGrid: self grid!

addPad: aPad onGrid: aPoint	"Add aPad (rounded to the nearest grid point) to the set	 of pads in the receiver.  Answer with the pad just		 inserted."	| pad |	pad _ aPad grid: aPoint.	self pads add: pad.	^pad!

addTrack: aTrack 	"Add aTrack to the set of tracks in the receiver.  Answer with	 the track just added."	^self addTrack: aTrack onGrid: self grid!

addTrack: aTrack onGrid: aPoint	"Add aTrack (rounded to the nearest grid point) to the set	 of tracks in the receiver.  Answer with the track just inserted."	| track |	track _	 aTrack grid: aPoint.	track isZeroLength ifTrue: [^nil] ifFalse: [self tracks add: track].	^track! !!PrintedCircuit methodsFor: 'removing'!

removePad: aPoint	"Remove the pad at aPoint from the receiver's list of pads."	self pads remove: (self pads detect: [:each | aPoint = each position])!

removeTrackSegment: anArray	"Remove the TrackSegment indicated by anArray from the	 receiver's list of tracks."	| start end |	start _ anArray at: 1.	end _ anArray at: 2.	self tracks remove:		(self tracks detect: [:each |				(start = each startPoint and: [end = each endPoint])			 or: [start = each endPoint and: [end = each startPoint]]])! !!PrintedCircuit methodsFor: 'file handling'!

bottomDrawerItemFrom: aFileStream 	"Gets a BottomDrawer Item representation from aFileStream. 	 Adds the item returned to the appropriate set (tracks or pads)."	| aChar |	aChar _ aFileStream next.	[aChar = $P | (aChar = $X)] whileFalse: [aChar _ aFileStream next].	aChar = $P ifTrue: [		self addPad: (Pad bottomDrawerPadFrom: aFileStream)].	aChar = $X ifTrue: [		self addTrack: (TrackSegment bottomDrawerTrackFrom: aFileStream)]!

readBottomDrawerFile: aFilename	"Initialize the receiver from the file indicated by aFilename."	| file |	file _ FileStream fileNamed: aFilename.	file text.	file readOnly.	file reset.	Cursor read showWhile: [		[file atEnd] whileFalse: [self bottomDrawerItemFrom: file]].	file close.	self changed: #all!

writeBottomDrawerOn: aFilename 	"Writes a BottomDrawer representation of the receiver	 on the named file."	| file temp routeNumber |	file _ FileStream fileNamed: aFilename.	file text.	Cursor wait showWhile: [		temp _ self tracks asSortedCollection.		temp addAll: self pads].	routeNumber _ 0.	Cursor write showWhile: [		temp do: [:each |			routeNumber _ routeNumber + 1.			file nextPutAll: routeNumber printString.			each writeBottomDrawerOn: file]].	file close!

writeBottomDrawerPadsOn: aFilename 	"Writes a BottomDrawer representation of the receiver	 (pads only) on the named file."	| file temp routeNumber |	file _ FileStream fileNamed: aFilename.	file text.	Cursor wait showWhile: [temp _ self pads asSortedCollection].	routeNumber _ 0.	Cursor write showWhile: [		temp do: [:each |			routeNumber _ routeNumber + 1.			file nextPutAll: routeNumber printString.			each writeBottomDrawerOn: file]].	file close!

writeBottomDrawerTracksOn: aFilename 	"Writes a BottomDrawer representation of the receiver	 (tracks only) on the named file."	| file temp routeNumber |	file _ FileStream fileNamed: aFilename.	file text.	Cursor wait showWhile: [temp _ self tracks asSortedCollection].	routeNumber _ 0.	Cursor write showWhile: [		temp do: [:each |			routeNumber _ routeNumber + 1.			file nextPutAll: routeNumber printString.			each writeBottomDrawerOn: file]].	file close! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!PrintedCircuit class	instanceVariableNames: ''!!PrintedCircuit class methodsFor: 'instance creation'!

fromBottomDrawerFile: aFilename	"Create a new printed circuit board from the bottom drawer	 file given."	^self new readBottomDrawerFile: aFilename!

new	"Create a new printed circuit board, with no pads or tracks."	^super new initialize! !!PrintedCircuit class methodsFor: 'class initialization'!

initialize	"Initialize various default values."	"PrintedCircuit initialize."	DefaultGrid _ 25@25.! !PrintedCircuit initialize!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:35:49 pm'!StandardSystemController subclass: #PcbTopController	instanceVariableNames: ''	classVariableNames: ''	poolDictionaries: ''	category: 'Printed-Circuits'!PcbTopController comment:'Instances of me are used instead of StandardSystemControlleras the controller for the top view for a PrintedCircuitView  I changethe functionality of some of the bluee button menu items.'!!PcbTopController methodsFor: 'menu messages'!

move	"Ask the user to designate a new origin position for the receiver's view.	 Override here to prevent a complete redraw after a move."	| labelForm labelOrigin viewBackground cursorPoint oldCursorPoint screenArea oldArea |	view deEmphasize.	sensor cursorPoint: view labelDisplayBox origin.	CacheBitmaps & view displayForm notNil	  ifFalse: [		labelForm _ Form fromDisplay: (view labelDisplayBox).		view erase.		Cursor origin showWhile:			[labelForm follow: [sensor cursorPoint] while: [sensor noButtonPressed]].		labelOrigin _ sensor cursorPoint.		view align: view labelDisplayBox origin			with: labelOrigin.		view displayEmphasized]	  ifTrue: [		Cursor origin showWhile: [			oldCursorPoint _ sensor cursorPoint.			oldArea _ view computeBoundingRectangleSet.			[sensor noButtonPressed] whileTrue: [				cursorPoint _ sensor cursorPoint.				cursorPoint ~= oldCursorPoint ifTrue:					[view align: oldCursorPoint with: cursorPoint.					 screenArea _ view computeBoundingRectangleSet.					 oldCursorPoint _ cursorPoint.					 view display.					 ScheduledControllers						displayViewsThrough: (oldArea difference: screenArea)						on: Display						excluding: view.					 oldArea _ screenArea]]].		view displayEmphasized.		sensor cursorPoint: view displayBox center.		sensor waitNoButton]!

redisplay	"Re-display the current view only.  Override for PCB views."		self view displayBorder.	self view displaySubViews.	self view emphasize.	self view emphasizeLabel! !'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:38:34 pm'!View subclass: #PrintedCircuitView	instanceVariableNames: 'currentPadForm currentPadDiameter currentTrackWidth currentTrackForm currentPads currentTracks currentTransformation gridFlag '	classVariableNames: 'DefaultWindow '	poolDictionaries: ''	category: 'Printed-Circuits'!PrintedCircuitView comment:'I represent a view on a PrintedCircuit.  I maintain a cache ofthe parts of the underlying model which can be seen at presentin instance variables currentPads and currentTracks.  I keep the currentPad and Track sizes in currentPadForm/currentPadDiameter andcurrentTrackForm/currentTrackWidth respectively.'!!PrintedCircuitView methodsFor: 'initialize-release'!

initialize	"Initialize some instance variables."	super initialize.	self grid: false.		"Don't display the grid."!

release	currentTracks release.	currentPads release.	currentPadForm release.	currentTrackForm release.	currentPadDiameter release.	currentTrackWidth release. 	currentTransformation release.	gridFlag release.	super release! !!PrintedCircuitView methodsFor: 'accessing'!

grid	"Answer whether the grid is to be displayed."	^gridFlag!

grid: aBoolean	"The grid is to be displayed if aBoolean is true."	gridFlag _ aBoolean!

nearestPadTo: aPoint 	"Answer with the pad which is the nearest to aPoint in the currently 	 displayed pads, or nil if there is no sufficiently close pad."	| grid box collection sortedCollection |	grid _ self model grid scaleBy: self displayTransformation scale.	box _ Rectangle origin: aPoint - grid corner: aPoint + grid.	collection _ currentPads select: [:each | box containsPoint: each].	collection size = 0 ifTrue: [^nil].	sortedCollection _ collection asSortedCollection: [:first :second |		(first - aPoint) abs r < (second - aPoint) abs r].	^collection first!

nearestTrackSegmentTo: aPoint 	"Answer with the TrackSegment which is the nearest one	 to aPoint in the currently displayed collection of TrackSegments,	 or nil if there is no sufficiently close TrackSegment."	| eachDistance closest closestDistance |	closestDistance _ 7.		"Max distance - seems about right."	currentTracks do: [:each |		eachDistance _ aPoint dist:			(aPoint pointNearestLine: each startPoint to: each endPoint).		eachDistance < closestDistance ifTrue: [			closestDistance _ eachDistance.			closest _ each]].	closestDistance < 7 ifTrue: [^closest] ifFalse: [^nil]! !!PrintedCircuitView methodsFor: 'displaying'!

displayGrid	"Check whether the grid should be displayed.  If so, display the	 grid associated with the model."	self grid ifTrue: [		(self model grid scaleBy: self displayTransformation scale) < (8@8)		  ifTrue: [Transcript cr; show: 'Scale too small to display grid.']		  ifFalse: [self reallyDisplayGrid]]!

displayPad: aPad	"Display aPad.  Add it to the currently displayed pads."	currentPadDiameter = aPad diameter ifFalse: [		currentPadForm _ Form dotOfSize: aPad diameter.		currentPadDiameter _ aPad diameter].	currentPads add: aPad.	currentPadForm							displayOn: Display		at: aPad position		clippingBox: self insetDisplayBox		rule: Form paint		mask: Form black!

displayPads	"Display the pads associated with the model."	currentPadDiameter _ 0.	currentPads do: [:each |		(currentPadDiameter = each diameter)		ifFalse: [			currentPadForm _ Form dotOfSize: each diameter.			currentPadDiameter _ each diameter].		currentPadForm								displayOn: Display			at: each position			clippingBox: self insetDisplayBox			rule: Form paint			mask: Form black]!

displayParts	"Display the grid, pads and tracks associated with the model."	self displayGrid.	self displayPads.	self displayTracks!

displayRubberBandFrom: startPoint to: endPoint 	"Display a rubber-band line from the nearest 	 point to startPoint on the model's grid, to the nearest 	 point to endPoint on the model's grid.  Answer	 the real end point."	^self		displayRubberBandFrom: startPoint		to: endPoint		onGrid: self model grid!

displayTrack: aTrackSegment.	"Display aTrackSegment.  Add it to the currently displayed tracks."	currentTrackWidth = aTrackSegment width ifFalse: [		currentTrackForm _ Pen new defaultNib: aTrackSegment width.		currentTrackForm frame: self insetDisplayBox.		currentTrackWidth _ aTrackSegment width].	currentTracks add: aTrackSegment.	currentTrackForm place: aTrackSegment startPoint.	currentTrackForm goto: aTrackSegment endPoint!

displayTracks	"Display the track segments associated with the model."	currentTrackWidth _ 0.	currentTracks do: [:each |		currentTrackWidth = each width		ifFalse: [			currentTrackForm _ Pen new defaultNib: each width.			currentTrackForm frame: self insetDisplayBox.			currentTrackWidth _ each width].		currentTrackForm place: each startPoint.		currentTrackForm goto: each endPoint]!

displayView	"Recalculate the displayed pads and tracks, then display the model."	(currentTransformation == self displayTransformation)	ifFalse: [		Cursor wait showWhile: [			currentPads _ nil.			currentPads _ self doTransformation: self model pads.			currentTracks _ nil.			currentTracks _ self doTransformation: self model tracks.			currentTransformation _ self displayTransformation]].	self displayParts!

removePad: aPoint	"Remove the displayed Pad at aPoint.  Remove it from the set	 of currently displayed pads."	| oldPad form |	oldPad _ self findOldPad: aPoint.	currentPads remove: oldPad.	self deleteDisplayedPad: oldPad.	self restoreGridAt: aPoint!

removeTrack: anArray	"Remove the TrackSegment indicated by anArray."	| oldTrackSegment |	oldTrackSegment _ self findOldTrack: anArray.	currentTracks remove: oldTrackSegment.	self deleteDisplayedTrack: oldTrackSegment!

update: aParameter	"The model has changed.  If aParameter is my model, redisplay	 all parts of the model from the currently displayed collection.  If aParameter	 is #all, completely re-create the display from the underlying model.	 If aParameter is #grid, just redisplay the parts necessary for the grid.	 If aParameter is a Pad, just display this pad.  If aParameter is a Point,	 remove the pad at this location.  If aParameter is a TrackSegment,	 just display this track.  If aParameter is an Array, remove the	 track represented by this array."	(self topView containsPoint: self controller sensor cursorPoint) ifFalse: [		self topView releaseSavedForms].	aParameter == self model ifTrue: [^self display].	aParameter == #all ifTrue: [		currentTransformation _ nil.		self topView releaseSavedForms.		^self display].	aParameter == #grid ifTrue: [		self grid ifTrue: [^self displayGrid] ifFalse: [^self display]].	(aParameter isMemberOf: Pad) ifTrue: [		^self displayPad: (self displayTransform: aParameter)].	(aParameter isMemberOf: Point) ifTrue: [		^self removePad: (self displayTransform: aParameter)].	(aParameter isMemberOf: TrackSegment) ifTrue: [		^self displayTrack: (self displayTransform: aParameter)].	(aParameter isMemberOf: Array) ifTrue: [		^self removeTrack: (Array				with: (self displayTransform: (aParameter at: 1))				with: (self displayTransform: (aParameter at: 2)))]! !!PrintedCircuitView methodsFor: 'display transformation'!

displayTransform: anObject 	"Override to use truncation rather than rounding."	^(self displayTransformation applyTo: anObject) truncated! !!PrintedCircuitView methodsFor: 'window access'!

setDefaultWindow	"Set the receiver's window to be the default size."	self window: DefaultWindow viewport: Display boundingBox!

setNewWindow	"Get a rectangle from the user, and change the receiver's window	 using this."	| rect tr |	rect _ Rectangle fromUser.	tr _ (self inverseDisplayTransform: rect origin)			corner: (self inverseDisplayTransform: rect corner).	self window: tr viewport: Display boundingBox! !!PrintedCircuitView methodsFor: 'controller access'!

defaultControllerClass	^PrintedCircuitController! !!PrintedCircuitView methodsFor: 'private'!

deleteDisplayedPad: aPad	"Overwrite the displayed version of aPad."	| form |	form _ Form dotOfSize: aPad diameter + 1.	form white.	form		displayOn: Display		at: aPad position		clippingBox: self insetDisplayBox		rule: Form over		mask: Form black!

deleteDisplayedTrack: aTrackSegment	"Overwrite the displayed version of aTrackSegment"	| pen |	pen _ Pen new defaultNib: aTrackSegment width + 1.	pen frame: self insetDisplayBox.	pen white.	pen combinationRule: Form over.	pen place: aTrackSegment startPoint.	pen goto: aTrackSegment endPoint!

displayRubberBandFrom: startPoint to: endPoint onGrid: aGrid	"Display a rubber-band, XOR thin line from the nearest	 point to startPoint on aGrid, to the nearest point to	 endPoint on aGrid.  The line is constrained to be at a	 multiple of 45 degrees.  Answer with the real end point	 so located."	| pen realEndPoint |	pen _ Pen new combinationRule: Form reverse.	pen frame: self insetDisplayBox.	pen place: (self displayTransform: (startPoint grid: aGrid)).	realEndPoint _ endPoint		nearestTo45DegreeLineThrough: (startPoint grid: aGrid) onGrid: aGrid.	pen goto: (self displayTransform: realEndPoint).	^realEndPoint!

doTransformation: aCollection	"Answer with a transformed and sorted collection containing	 the elements in aCollection."	| corner origin collection |	corner _ self window corner.	origin _ self window origin.	collection _ aCollection reject:		[:each | each refPoint > corner or: [each refPoint < origin]].	^(collection collect: [:each | self displayTransform: each])		asSortedCollection: [:first :second | first size > second size]!

findOldPad: aPoint	"Find the old displayed Pad corresponding to aPoint.  If not	 found, perform aBlock."	| box |	box _ Rectangle origin: (aPoint - (1@1)) corner: (aPoint + (2@2)).	^currentPads detect: [:each | box containsPoint: each position]!

findOldTrack: anArray 	"Find the old displayed TrackSegment corresponding to anArray."	| startBox endBox |	startBox _ Rectangle			origin: (anArray at: 1) - (1 @ 1)			corner: (anArray at: 1) + (2 @ 2).	endBox _ Rectangle			origin: (anArray at: 2) - (1 @ 1)			corner: (anArray at: 2) + (2 @ 2).	^currentTracks detect: [:each |			((startBox containsPoint: each startPoint)					and: [endBox containsPoint: each endPoint])			or: [(startBox containsPoint: each endPoint)					and: [endBox containsPoint: each startPoint]]]!

reallyDisplayGrid	"Actually display the grid."	| form xStart xEnd yStart yEnd |	form _ Form dotOfSize: 1.	xStart _ (self window origin grid: self model grid) x.	xEnd _ (self window corner grid: self model grid) x.	yStart _ (self window origin grid: self model grid) y.	yEnd _ (self window corner grid: self model grid) y.	xStart to: xEnd by: self model grid x do: [:x |		yStart to: yEnd by: self model grid y do: [:y |			form				displayOn: Display				at: (self displayTransform: x@y)				clippingBox: self insetDisplayBox]]!

restoreGridAt: aPoint	"If the grid is enabled, restore the grid point at aPoint."	| form | 	self grid ifTrue: [		form _ Form dotOfSize: 1.		form			displayOn: Display			at: aPoint			clippingBox: self insetDisplayBox]! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!PrintedCircuitView class	instanceVariableNames: ''!!PrintedCircuitView class methodsFor: 'instance creation'!

open	"Create and schedule a printed circuit editor on a new	 printed circuit board."	"PrintedCircuitView open."	self openOn: PrintedCircuit new!

openOn: aPrintedCircuit 	"Create and schedule a new instance of me on aPrintedCircuit."	"PrintedCircuitView openOn: TestPCB."	| topView pcbView |	topView _ StandardSystemView				model: aPrintedCircuit				label: 'Printed Circuit Editor'				minimumSize: 250@200.	topView controller: PcbTopController new.	pcbView _ self new model: aPrintedCircuit.	pcbView borderWidth: 1.	pcbView insideColor: Form white.	pcbView setDefaultWindow.	topView addSubView: pcbView.	topView controller open!

openOnFile: aFilename	"Create and schedule a new instance of me on a new printed	 circuit with a bottom drawer representation in the file given	 by aFilename."	"PrintedCircuitView openOnFile: 'test.bd'."	self openOn: (PrintedCircuit fromBottomDrawerFile: aFilename)! !!PrintedCircuitView class methodsFor: 'class initialization'!

initialize	"Initialize various default values."	"PrintedCircuitView initialize."	DefaultWindow _ -20@-20 extent: 5200@3000! !!PrintedCircuitView class methodsFor: 'class access'!

defaultWindow: aRectangle	"Make aRectangle the default window."	DefaultWindow _ aRectangle! !!PrintedCircuitView class methodsFor: 'examples'!

exampleWorkspace	"Select and execute the expressions here to create and	 manipulate PrintedCircuits"	"	Smalltalk at: #TestPCB put: PrintedCircuit new.	TestPCB readBottomDrawerFile: 'test.bd'.	TestPCB readBottomDrawerFile: 'slim.pad.bd'.	PrintedCircuitView openOn: TestPCB.	TestPCB writeBottomDrawerOn: 'new.bd'.	Smalltalk removeKey: #TestPCB.	Smalltalk garbageCollect.	"! !PrintedCircuitView initialize!'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:39:05 pm'!MouseMenuController subclass: #PrintedCircuitController	instanceVariableNames: 'currentTrackWidth currentPadSize redButtonFunction '	classVariableNames: 'DefaultPadSize DefaultRedButtonFunction DefaultTrackWidth PCBYellowButtonMenu PCBYellowButtonMessages '	poolDictionaries: ''	category: 'Printed-Circuits'!PrintedCircuitController comment:'I represent a controller for a PrintedCircuitView.  I maintain acurrentTrackWidth and a currentPadSize, which are used tocreate new Pads and TrackSegments.  The instance variableredButtonFunction indicates which operation (add/delete Pad/Track)is performed by the red mouse button.'!!PrintedCircuitController methodsFor: 'initialize-release'!

initialize	"Initialize the yellow button menus and the current pad	 and track sizes.  Initialize the red button action."	super initialize.	self		yellowButtonMenu: PCBYellowButtonMenu		yellowButtonMessages: PCBYellowButtonMessages.	self trackWidth: DefaultTrackWidth.	self padSize: DefaultPadSize.	self redButtonFunction: DefaultRedButtonFunction! !!PrintedCircuitController methodsFor: 'accessing'!

padSize	"Answer with the current pad size."	^currentPadSize!

padSize: aNumber	"Set the current pad size."	currentPadSize _ aNumber!

redButtonFunction	"Answer with the current red button function (a Symbol)."	^redButtonFunction!

redButtonFunction: aSymbol	"Set the current red button function to a Symbol."	redButtonFunction _ aSymbol!

trackWidth	"Answer with the current track width."	^currentTrackWidth!

trackWidth: aNumber	"Set the current track width."	currentTrackWidth _ aNumber! !!PrintedCircuitController methodsFor: 'menu messages'!

addPads	"Set the current red button action to be adding Pads."	self redButtonFunction: #addPads!

addTracks	"Set the current red button action to be adding Tracks."	self redButtonFunction: #addTracks!

changeGridSize	"Prompt the user for a new grid size, and set this value as the	 current grid size."	| aGridSize aNumber |	aGridSize _ FillInTheBlank				request: ' New Grid Size? '				initialAnswer: self model grid x printString.	aGridSize isEmpty ifFalse: [		aNumber _ Number readFrom: (ReadStream on: aGridSize).		self model grid: (aNumber@aNumber).		self view grid ifTrue: [self model changed]]!

changePadSize	"Prompt the user for a new pad size, and set this value as the	 current pad size."	| aPadSize |	aPadSize _ FillInTheBlank			request: ' New Pad Size? '			initialAnswer: (self padSize printString) .	aPadSize isEmpty ifFalse: [		self padSize: (Number readFrom: (ReadStream on: aPadSize))]!

changeTrackWidth	"Prompt the user for a new track width, and set this value as the	 current track size."	| aTrackWidth |	aTrackWidth _ FillInTheBlank			request: ' New Track Width? '			initialAnswer: (self trackWidth printString).	aTrackWidth isEmpty ifFalse: [		self trackWidth: (Number readFrom: (ReadStream on: aTrackWidth))]!

changeWindow	"Prompt the user for a new window."	self view setNewWindow.	self view update: self model!

defaultWindow	"Set my view's window to its default value."	self view setDefaultWindow.	self view update: self model!

deletePads	"Set the current red button action to be deleting Pads."	self redButtonFunction: #deletePads!

deleteTracks	"Set the current red button action to be deleting TrackSegments."	self redButtonFunction: #deleteTracks!

toggleGrid	"Toggle the displaying of the grid associated with my model."	self view grid: (self view grid not).	self view update: #grid! !!PrintedCircuitController methodsFor: 'basic control sequence'!

controlInitialize	Cursor crossHair show!

controlTerminate	Cursor normal show! !!PrintedCircuitController methodsFor: 'control defaults'!

isControlActive	^(view containsPoint: sensor cursorPoint) & sensor blueButtonPressed not! !!PrintedCircuitController methodsFor: 'button activities'!

action: aSymbol at: aPoint	"Perform the action indicated by aSymbol at aPoint."	aSymbol == #addPads ifTrue: [^self addPadAt: aPoint].	aSymbol == #deletePads ifTrue: [^self removePadAt: aPoint].	aSymbol == #addTracks ifTrue: [^self addTrackAt: aPoint].	aSymbol == #deleteTracks ifTrue: [^self removeTrackAt: aPoint]!

addPadAt: aPoint 	"Add a Pad of the current size to the printed circuit represented 	 by my model."	| newPad |	newPad _ Pad				point: (self view inverseDisplayTransform: aPoint)				diameter: self padSize.	self model changed: (self model addPad: newPad).	self sensor waitNoButton!

addTrackAt: aPoint	"Add a track of the current size to the collection of tracks	 represented by my model, starting at aPoint.  The end point	 is indicated by the user and is locked to the view's grid."	| realStartPoint currentPoint realCurrentPoint endPoint newTrack |	realStartPoint _ self view inverseDisplayTransform: aPoint.	currentPoint _ self sensor cursorPoint.	realCurrentPoint _ self view inverseDisplayTransform: currentPoint.	self view		displayRubberBandFrom: realStartPoint		to: realCurrentPoint.	[self sensor redButtonPressed] whileTrue: [		(self sensor cursorPoint = currentPoint) ifFalse: [			self view				displayRubberBandFrom: realStartPoint				to: realCurrentPoint.			currentPoint _ self sensor cursorPoint.			realCurrentPoint _ self view inverseDisplayTransform: currentPoint.			self view				displayRubberBandFrom: realStartPoint				to: realCurrentPoint]].	endPoint _ self view		displayRubberBandFrom: realStartPoint		to: realCurrentPoint.	newTrack _ TrackSegment		from: realStartPoint		to: endPoint		width: self trackWidth.	self model changed: (self model addTrack: newTrack)!

redButtonActivity	"Perform the current red button activity at the current input point."	self sensor redButtonPressed ifTrue: [		self action: self redButtonFunction at: self sensor cursorPoint]!

removePadAt: aPoint 	"Remove the nearest Pad in the printed circuit represented 	 by my model.  Search in the view's currently displayed pads	 for the nearest one."	| nearest pos |	nearest _ self view nearestPadTo: aPoint.	nearest isNil ifFalse: [		pos _ (self view inverseDisplayTransform: nearest position)					grid: self model grid.		self model removePad: pos.		self model changed: pos].	self sensor waitNoButton!

removeTrackAt: aPoint	"Remove the nearest track segment to aPoint, provided it	 is close enough."	| nearest anArray |	nearest _ self view nearestTrackSegmentTo: aPoint.	nearest isNil ifFalse: [		anArray _ Array		with:		 ((self view inverseDisplayTransform: nearest startPoint) grid: self model grid)		with:		 ((self view inverseDisplayTransform: nearest endPoint) grid: self model grid).		self model removeTrackSegment: anArray.		self model changed: anArray].	self sensor waitNoButton!

yellowButtonActivity	"Change the cursor while the yellow button menu is active."	Cursor normal showWhile: [super yellowButtonActivity]! !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!PrintedCircuitController class	instanceVariableNames: ''!!PrintedCircuitController class methodsFor: 'class initialization'!

initialize	"Initialize the yellow button menus, and various default values."	PCBYellowButtonMenu _ PopUpMenu		labels: 'add pads\add tracks\delete pads\delete tracks\track width\pad size\grid size\toggle grid\zoom\un-zoom' withCRs		lines: #(4 6 8).	PCBYellowButtonMessages _		#(addPads addTracks deletePads deleteTracks		   changeTrackWidth changePadSize changeGridSize toggleGrid		   changeWindow defaultWindow).	DefaultPadSize _ Pad defaultDiameter.	DefaultTrackWidth _ TrackSegment defaultWidth.	DefaultRedButtonFunction _ #addPads.	"PrintedCircuitController initialize."! !PrintedCircuitController initialize!
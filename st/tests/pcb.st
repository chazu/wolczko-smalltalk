'From Smalltalk-80, version 2, of April 1, 1983 on 29 July 1987 at 2:24:31 pm'!

size

includes: aNumber

isMultipleOf: aNumber 

notMultipleOf: aNumber 

truncated

nearestTo45DegreeLineThrough: refPoint onGrid: aGridPoint

pointNearestLine: point1 to: point2

truncated

refPoint

refX

refY

size

writeBottomDrawerOn: aFileStream

getIntegerFrom: aFileStream 

getSlashFrom: aFileStream

diameter

diameter: aNumber

position

size

isZeroDiameter

= anObject

rounded

truncated

scaleBy: aPoint 

translateBy: delta 

grid: aPoint

printOn: aStream 

asBottomDrawer

bottomDrawerPadSize: aDiameter 

setX: xPad setY: yPad setDiameter: diameterPad

bottomDrawerPadFrom: aFileStream 

point: aPoint 

point: aPoint diameter: aNumber

x: xInteger y: yInteger 

x: xInteger y: yInteger diameter: diameterInteger 

initialize.

defaultDiameter

endPoint

size

startPoint

width

x1

x2

y1

y2

isZeroLength

= aTrackSegment 

sameEndAs: aTrackSegment

sameLineAs: aTrackSegment

sameStartAs: aTrackSegment

sameWidthAs: aTrackSegment

rounded

truncated

scaleBy: aPoint 

translateBy: delta 

grid: aPoint 

printOn: aStream 

asBottomDrawer

bottomDrawerTrackSize: aTrackWidth 

setX1: x1 setY1: y1 setX2: x2 setY2: y2 setWidth: trackWidth 

bottomDrawerTrackFrom: aFileStream 

from: point1 to: point2 

from: point1 to: point2 width: widthInteger 

x1: x1 y1: y1 x2: x2 y2: y2 

x1: x1 y1: y1 x2: x2 y2: y2 width: widthInteger 

initialize

defaultWidth

initialize

release

restart

grid

grid: aPoint

pads

tracks

addPad: aPad

addPad: aPad onGrid: aPoint

addTrack: aTrack 

addTrack: aTrack onGrid: aPoint

removePad: aPoint

removeTrackSegment: anArray

bottomDrawerItemFrom: aFileStream 

readBottomDrawerFile: aFilename

writeBottomDrawerOn: aFilename 

writeBottomDrawerPadsOn: aFilename 

writeBottomDrawerTracksOn: aFilename 

fromBottomDrawerFile: aFilename

new

initialize

move

redisplay

initialize

release

grid

grid: aBoolean

nearestPadTo: aPoint 

nearestTrackSegmentTo: aPoint 

displayGrid

displayPad: aPad

displayPads

displayParts

displayRubberBandFrom: startPoint to: endPoint 

displayTrack: aTrackSegment.

displayTracks

displayView

removePad: aPoint

removeTrack: anArray

update: aParameter

displayTransform: anObject 

setDefaultWindow

setNewWindow

defaultControllerClass

deleteDisplayedPad: aPad

deleteDisplayedTrack: aTrackSegment

displayRubberBandFrom: startPoint to: endPoint onGrid: aGrid

doTransformation: aCollection

findOldPad: aPoint

findOldTrack: anArray 

reallyDisplayGrid

restoreGridAt: aPoint

open

openOn: aPrintedCircuit 

openOnFile: aFilename

initialize

defaultWindow: aRectangle

exampleWorkspace

initialize

padSize

padSize: aNumber

redButtonFunction

redButtonFunction: aSymbol

trackWidth

trackWidth: aNumber

addPads

addTracks

changeGridSize

changePadSize

changeTrackWidth

changeWindow

defaultWindow

deletePads

deleteTracks

toggleGrid

controlInitialize

controlTerminate

isControlActive

action: aSymbol at: aPoint

addPadAt: aPoint 

addTrackAt: aPoint

redButtonActivity

removePadAt: aPoint 

removeTrackAt: aPoint

yellowButtonActivity

initialize
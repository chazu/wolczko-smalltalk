'From Smalltalk-80, version 2, of April 1, 1983 on 18 February 1987 at 4:46:06 pm'!Object subclass: #Future	instanceVariableNames: 'result semaphore '	classVariableNames: ''	poolDictionaries: ''	category: 'Kernel-Processes'!Future comment:'I represent an execution in progress.  Any messages sent to me are delayed until execution has completed.'!!Future methodsFor: 'synchronising'!

doesNotUnderstand: aMessage	semaphore wait.	semaphore signal.	"Wake up anything else that might be waiting"	^result perform: aMessage selector withArguments: aMessage arguments! !!Future methodsFor: 'evaluating'!

block: aBlock	"Execute aBlock in parallel with whatever called me, but ensure that any messages sent to me before execution of the block has terminated are suspended until it has terminated."	semaphore _ Semaphore new.	[result _ aBlock value.  semaphore signal] fork!

block: aBlock value: aValue	semaphore _ Semaphore new.	[result _ aBlock value: aValue.  semaphore signal] fork!

 !"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!Future class	instanceVariableNames: ''!!Future class methodsFor: 'examples'!

example	| fac |	fac _ [100 factorial] futureValue.	Transcript show: 'evaluating factorial...'.	Transcript show: fac printString	"Future example"! !!Future class methodsFor: 'class initialization'!

initialize	superclass _ nil 	"must avoid the checks"! !Future initialize!!BlockContext methodsFor: 'parallel evaluation'!

futureValue	"Fork a synchronised evaluation of myself"	^Future new block: self! !
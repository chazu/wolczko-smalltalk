Object subclass: #RBObject
	instanceVariableNames: ''
	classVariableNames: 'DeviceA DeviceB DevicePacketKind HandlerA HandlerB Idler NoTask NoWork Worker WorkPacketKind '
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!RBObject methodsFor: 'utilities'!

append: packet head: queueHead
	| mouse link |
	packet link: NoWork.
	NoWork == queueHead ifTrue: [^packet].
	mouse _ queueHead.
	[NoWork == (link _ mouse link)]
		whileFalse: [mouse _ link].
	mouse link: packet.
	^queueHead! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

RBObject class
	instanceVariableNames: ''!


!RBObject class methodsFor: 'initialize'!

initialize
	"RBObject initialize"
	DeviceA _ 5.
	DeviceB _ 6.
	DevicePacketKind _ 1.
	HandlerA _ 3.
	HandlerB _ 4.
	Idler _ 1.
	NoWork _ nil.
	NoTask _ nil.
	Worker _ 2.
	WorkPacketKind _ 2! !

RBObject initialize!


RBObject subclass: #Packet
	instanceVariableNames: 'link identity kind datum data '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!Packet methodsFor: 'initialize'!

link: aLink identity: anIdentity kind: aKind 
	link _ aLink.
	identity _ anIdentity. 
	kind _ aKind.
	datum _ 1.
	data _ Array new: 4 withAll: 0! !

!Packet methodsFor: 'accessing'!

data
	^data!

datum
	^datum!

datum:  someData
	datum _ someData!

identity
	^identity!

identity: anIdentity
	identity _ anIdentity!

kind
	^kind!

link
	^link!

link: aWorkQueue 
	link _ aWorkQueue! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

Packet class
	instanceVariableNames: ''!


!Packet class methodsFor: 'instance creation'!

create: link identity: identity kind: kind 
	^super new
		link: link
		identity: identity
		kind: kind! !

RBObject subclass: #RichardsBenchmark
	instanceVariableNames: 'taskList currentTask currentTaskIdentity taskTable tracing layout queuePacketCount holdCount '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!RichardsBenchmark methodsFor: 'creation'!

createDevice: identity priority: priority work: work state: state 
	| data functionWork |
	data _ DeviceTaskDataRecord create.
	self
		createTask: identity
		priority: priority
		work: work
		state: state
		function: 
			[:work :word | 
			data _ word.
			functionWork _ work.
			NoWork == functionWork
				ifTrue:
					[NoWork == (functionWork _ data pending)
						ifTrue: [self wait]
						ifFalse: 
							[data pending: NoWork.
							self queuePacket: functionWork]]
				ifFalse: 
					[data pending: functionWork.
					tracing
						ifTrue: 
							[self trace: functionWork datum].
					self holdSelf]]
		data: data!

createHandler: identity priority: priority work: work state: state 
	| data workPacket count devicePacket |
	data _ HandlerTaskDataRecord create.
	self
		createTask: identity
		priority: priority
		work: work
		state: state
		function: 
			[:work :word | 
			data _ word.
			NoWork == work
				ifFalse: [WorkPacketKind == work kind 
					ifTrue: [data workInAdd: work]
					ifFalse: [data deviceInAdd: work]].
			NoWork == (workPacket _ data workIn)
				ifTrue: [self wait]
				ifFalse: 
					[count _ workPacket datum.
					count > 4
						ifTrue: 
							[data workIn: workPacket link.
							self queuePacket: workPacket]
						ifFalse:
							[NoWork == (devicePacket _ data deviceIn)
								ifTrue: [self wait]
								ifFalse: 
									[data deviceIn: devicePacket link.
									devicePacket datum: (workPacket data at: count).
									workPacket datum: count + 1.
									self queuePacket: devicePacket]]]]
		data: data!

createIdler: identity priority: priority work: work state: state 
	| data |
	data _ IdleTaskDataRecord create.
	self
		createTask: identity
		priority: priority
		work: work
		state: state
		function: 
			[:work :word | 
			data _ word.
			data count: data count - 1.
			0 = data count
				ifTrue: [self holdSelf]
				ifFalse:
					[0 = (data control bitAnd: 1)
						ifTrue: 
							[data control: data control // 2.
							self release: DeviceA]
						ifFalse: 
							[data control: (data control // 2 bitXor: 53256).
							self release: DeviceB]]]
		data: data!

createPacket: link identity: identity kind: kind 
	^Packet
		create: link
		identity: identity
		kind: kind!

createTask: identity priority: priority work: work state: state function: aBlock data: data 
	| t |
	t _ TaskControlBlock
				link: taskList
				create: identity
				priority: priority
				initialWorkQueue: work
				initialState: state
				function: aBlock
				privateData: data.
	taskList _ t.
	taskTable at: identity put: t!

createWorker: identity priority: priority work: work state: state 
	| data |
	data _ WorkerTaskDataRecord create.
	self
		createTask: identity
		priority: priority
		work: work
		state: state
		function: 
			[:work :word | 
			data _ word.
			NoWork == work
				ifTrue: [self wait]
				ifFalse: 
					[data destination: (HandlerA = data destination
							ifTrue: [HandlerB]
							ifFalse: [HandlerA]).
					work identity: data destination.
					work datum: 1.
					1 to: 4 do: 
						[:i | 
						data count: data count + 1.
						data count > 26 ifTrue: [data count: 1].
						work data at: i put: $A asInteger + data count - 1].
					self queuePacket: work]]
		data: data! !

!RichardsBenchmark methodsFor: 'private'!

findTask: identity 
	| t |
	t _ taskTable at: identity.
	NoTask == t ifTrue: [self error: 'findTask failed'].
	^t!

holdSelf
	holdCount _ holdCount + 1.
	currentTask taskHolding: true.
	^currentTask link!

initScheduler
	queuePacketCount _ holdCount _ 0.
	taskTable _ Array new: 6 withAll: NoTask.
	taskList _ NoTask!

initTrace
	BinaryChoice
		message: 'Trace?' 
		displayAt: Sensor mousePoint
		centered: true
		ifTrue: [tracing _ true] 
		ifFalse: [tracing _ false].

	layout _ 0!

queuePacket: packet 
	| t |
	t _ self findTask: packet identity.
	NoTask == t ifTrue: [^NoTask].
	queuePacketCount _ queuePacketCount + 1.
	packet link: NoWork.
	packet identity: currentTaskIdentity.
	^t addInput: packet checkPriority: currentTask!

release: identity 
	| t |
	t _ self findTask: identity.
	NoTask == t ifTrue: [^NoTask].
	t taskHolding: false.
	t priority > currentTask priority
		ifTrue: [^t]
		ifFalse: [^currentTask]!

trace: id 
	layout _ layout - 1.
	0 >= layout
		ifTrue: 
			[Transcript cr.
			layout _ 50].
	Transcript show: id printString!

wait 
	currentTask taskWaiting: true.
	^currentTask! !

!RichardsBenchmark methodsFor: 'scheduling'!

schedule
	currentTask _ taskList. 
	[NoTask == currentTask]
		whileFalse: 
			[currentTask isTaskHoldingOrWaiting
				ifTrue: [currentTask _ currentTask link]
				ifFalse: 
					[currentTaskIdentity _ currentTask identity.
					tracing ifTrue: [self trace: currentTaskIdentity].
					currentTask _ currentTask runTask]]! !

!RichardsBenchmark methodsFor: 'initialize'!

start
	| workQ mark1 mark2 mark3 mark4 |
	self initTrace; initScheduler.
	mark1 _ Time millisecondClockValue.
	tracing ifTrue: [Transcript show: 'Bench mark starting'; cr].
	self
		createIdler: Idler
		priority: 0
		work: NoWork
		state: TaskState running.
	workQ _ self
				createPacket: NoWork
				identity: Worker
				kind: WorkPacketKind.
	workQ _ self
				createPacket: workQ
				identity: Worker
				kind: WorkPacketKind.
	self
		createWorker: Worker
		priority: 1000
		work: workQ
		state: TaskState waitingWithPacket.
	workQ _ self
				createPacket: NoWork
				identity: DeviceA
				kind: DevicePacketKind.
	workQ _ self
				createPacket: workQ
				identity: DeviceA
				kind: DevicePacketKind.
	workQ _ self
				createPacket: workQ
				identity: DeviceA
				kind: DevicePacketKind.
	self
		createHandler: HandlerA
		priority: 2000
		work: workQ
		state: TaskState waitingWithPacket.
	workQ _ self
				createPacket: NoWork
				identity: DeviceB
				kind: DevicePacketKind.
	workQ _ self
				createPacket: workQ
				identity: DeviceB
				kind: DevicePacketKind.
	workQ _ self
				createPacket: workQ
				identity: DeviceB
				kind: DevicePacketKind.
	self
		createHandler: HandlerB
		priority: 3000
		work: workQ
		state: TaskState waitingWithPacket.
	self
		createDevice: DeviceA
		priority: 4000
		work: NoWork
		state: TaskState waiting.
	self
		createDevice: DeviceB
		priority: 5000
		work: NoWork
		state: TaskState waiting.
	tracing ifTrue: [Transcript show: 'Starting'; cr].
	mark2 _ Time millisecondClockValue.
	self schedule.
	mark3 _ Time millisecondClockValue.
	tracing
		ifTrue: 
			[Transcript show: 'Finished'; cr.
			Transcript show: 'QueuePacket count = ' , queuePacketCount printString , ' HoldCount = ' , holdCount printString; cr.
			Transcript cr; show: 'End of run'; cr].
	queuePacketCount = 23246 & (holdCount = 9297) ifFalse: [self error: 'wrong result'].
	mark4 _ Time millisecondClockValue.
	Transcript show: '***Scheduler time = ' , (mark3 - mark2) printString , ' Total time = ' , (mark4 - mark1) printString; cr! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

RichardsBenchmark class
	instanceVariableNames: ''!


!RichardsBenchmark class methodsFor: 'instance creation'!

start
	"RichardsBenchmark start"

	super new start! !

RBObject subclass: #TaskState
	instanceVariableNames: 'packetPending taskWaiting taskHolding '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!TaskState methodsFor: 'initialize'!

packetPending
	packetPending _ true.
	taskWaiting _ false.
	taskHolding _ false!

running
	packetPending _ taskWaiting _ taskHolding _ false!

waiting
	packetPending _ taskHolding _ false.
	taskWaiting _ true!

waitingWithPacket
	taskHolding _ false.
	taskWaiting _ packetPending _ true! !

!TaskState methodsFor: 'accessing'!

isPacketPending
	^packetPending!

isTaskHolding
	^taskHolding!

isTaskWaiting
	^taskWaiting!

taskHolding: aBoolean
	taskHolding _ aBoolean!

taskWaiting: aBoolean 
	taskWaiting _ aBoolean! !

!TaskState methodsFor: 'testing'!

isRunning
	^packetPending not and: [taskWaiting not and: [taskHolding not]]!

isTaskHoldingOrWaiting
	^taskHolding or: [packetPending not and: [taskWaiting]]!

isWaiting
	^packetPending not and: [taskWaiting and: [taskHolding not]]!

isWaitingWithPacket
	^packetPending and: [taskWaiting and: [taskHolding not]]! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

TaskState class
	instanceVariableNames: ''!


!TaskState class methodsFor: 'instance creation'!

packetPending
	^super new packetPending!

running
	^super new running!

waiting
	^super new waiting!

waitingWithPacket
	^super new waitingWithPacket! !

RBObject subclass: #DeviceTaskDataRecord
	instanceVariableNames: 'pending '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!DeviceTaskDataRecord methodsFor: 'initialize'!

create
	pending _ NoWork! !

!DeviceTaskDataRecord methodsFor: 'accessing'!

pending
	^pending!

pending: packet 
	pending _ packet! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

DeviceTaskDataRecord class
	instanceVariableNames: ''!


!DeviceTaskDataRecord class methodsFor: 'instance creation'!

create
	^super new create! !

RBObject subclass: #HandlerTaskDataRecord
	instanceVariableNames: 'workIn deviceIn '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!HandlerTaskDataRecord methodsFor: 'initialize'!

create
	workIn _ deviceIn _ NoWork! !

!HandlerTaskDataRecord methodsFor: 'accessing'!

deviceIn
	^deviceIn!

deviceIn: aPacket
	deviceIn _ aPacket!

deviceInAdd: packet
	deviceIn _ self append: packet head: deviceIn!

workIn
	^workIn!

workIn: aWorkQueue 
	workIn _ aWorkQueue!

workInAdd: packet
	workIn _ self append: packet head: workIn! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

HandlerTaskDataRecord class
	instanceVariableNames: ''!


!HandlerTaskDataRecord class methodsFor: 'instance creation'!

create
	^super new create! !

RBObject subclass: #WorkerTaskDataRecord
	instanceVariableNames: 'destination count '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!WorkerTaskDataRecord methodsFor: 'initialize'!

create
	destination _ HandlerA.
	count _ 0! !

!WorkerTaskDataRecord methodsFor: 'accessing'!

count
	^count!

count: aCount
	count _ aCount!

destination
	^destination!

destination: aHandler
	destination _ aHandler! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

WorkerTaskDataRecord class
	instanceVariableNames: ''!


!WorkerTaskDataRecord class methodsFor: 'instance creation'!

create
	^super new create! !

TaskState subclass: #TaskControlBlock
	instanceVariableNames: 'link identity priority input state function handle '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!TaskControlBlock methodsFor: 'initialize'!

link: aLink identity: anIdentity priority: aPriority initialWorkQueue: anInitialWorkQueue initialState: anInitialState function: aBlock privateData: aPrivateData 
	link _ aLink.
	identity _ anIdentity.
	priority _ aPriority.
	input _ anInitialWorkQueue.
	packetPending _ anInitialState isPacketPending.
	taskWaiting _ anInitialState isTaskWaiting.
	taskHolding _ anInitialState isTaskHolding.
	function _ aBlock.
	handle _ aPrivateData! !

!TaskControlBlock methodsFor: 'accessing'!

identity
	^identity!

link
	^link!

priority
	^priority! !

!TaskControlBlock methodsFor: 'scheduling'!

addInput: packet checkPriority: oldTask
	NoWork == input
		ifTrue: 
			[input _ packet.
			packetPending _ true.
			priority > oldTask priority ifTrue: [^self]]
		ifFalse: 
			[input _ self append: packet head: input].
	^oldTask!

runTask
	| message |
	self isWaitingWithPacket
		ifTrue: 
			[message _ input.
			input _ message link.
			NoWork == input
				ifTrue: [self running]
				ifFalse: [self packetPending]]
		ifFalse: [message _ NoWork].
	^function value: message value: handle! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

TaskControlBlock class
	instanceVariableNames: ''!


!TaskControlBlock class methodsFor: 'instance creation'!

link: link create: identity priority: priority initialWorkQueue: initialWorkQueue initialState: initialState function: aBlock privateData: privateData 
	^super new
		link: link
		identity: identity
		priority: priority
		initialWorkQueue: initialWorkQueue
		initialState: initialState
		function: aBlock
		privateData: privateData! !

RBObject subclass: #IdleTaskDataRecord
	instanceVariableNames: 'control count '
	classVariableNames: ''
	poolDictionaries: ''
	category: 'RichardsBenchmark'!


!IdleTaskDataRecord methodsFor: 'initialize'!

create
	control _ 1.
	count _ 10000! !

!IdleTaskDataRecord methodsFor: 'accessing'!

control
	^control!

control: aNumber
	control _ aNumber!

count
	^count!

count: aCount
	count _ aCount! !
"-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- "!

IdleTaskDataRecord class
	instanceVariableNames: ''!


!IdleTaskDataRecord class methodsFor: 'instance creation'!

create
	^super new create! !

# project-x-college

This is the C++ Source code part of the college project.

## Character classes

Characters are Pawns that have a mesh, collision, and built-in movement logic. They are responsible for all physical interaction between the player or AI and the world, and also implement basic networking and input models. They are designed for a vertically-oriented player representation that can walk, jump, fly, and swim through the world using CharacterMovementComponent.

![class uml diagram](https://github.com/pz64/project-x-college/blob/master/files/class-diagram.jpg)

### AProjectXCharacter

Inherits Built-in Parent class ACharacter.
This class is the base class for every character in the game. This class contains codes for handling keyboard inputs, Health system, Weapon and Inventory system and Interaction system.
There are functions and variables associated with each keybord input. When Player press a key, respective action is performed, or variable is set|unset. 
Health system manages the Health of the player and set the state from LIVING to DEAD according the value of the float variable Health. The variable is influenced by impact from weapon or Falling from height.
Weapon and inventory system manages the Shooting and Storing of weapon. The weapon is of type AActor and is the child of AInteractable class which manages the interaction of Objects with respect of the player. 
The interaction system manages every player and AI interaction with objects. This class uses its interface IInteractableInterface which sends and receive message between Character and weapon or between character and other interactive objects like Doors, Vehicles .. 

The animation and poses are controlled by a class called AAnimInstance or AnimationBlueprint. The variables set | unset by the character are used by the Animation blueprint to modify poses or play corresponding animation to the character.   

### AProjectXPlayerCharacter

Inherits AProjectXCharacter.
This class is used as the Player character. Even though the parent class is enough for this purpose, a separate class, child class, is kept to distinguish from AI characters. This class contain the actual reference of 3D mesh and other Player specific settings like camera position, skin etc.

### AProjectXAICharacter

Inherits AProjectXCharacter.
This class is controlled by the AIController rather than PlayerController. The only difereence is that instead of keyboard input this class uses Behavior tree input which is coded by the Developer. The AIController contain the components equivalent the eye and ear of an AI. With AIController, the abilities like sight, hearing can be given to AI.

### ASupportAICharacter,AEnemyAICharacter

Inherits AProjectXAICharacter.
Both of these classes differ only by the behavior tree used. Other wise both have same ability of doing everything. Due to the change in the behavior tree used, Both these AIs differs. Support AIs are meant to be supporting player, as a co-worker. Support AIs are provoked on the presence of Enemy AI and start to attack until death, it is same for the Enemy AI also. The AIs are designed in a way that they can play most of the attack themselves. 

## Behavior classes

Behavior classes are Actors that controls the behavior of AIs by calculating and planning the resources and for implementing several behaviors like Group behavior etc. There are no Specialized class in the engine for this purpose, so it’s completely written upon the bare AActor class. The behaviors that are implemented in the game are

* Group Behavior
* Combat Behavior

### Group Behavior

Group behavior is a layer of consciousness build upon AIs to take collective decision on performing several tasks including pickup of Weapons, and other Interactable objects. If individual consciousness is only present, then AI tends to have a greedy behavior in resource lacking cases. 
In this project we created a separate class that manages the group activity which ensure every AI should get minimum number of resource in lacking situation. Every Actor in the game, once spawned, are registered in the group behavior Actor. Registering simply means sending its instance to the group behavior actor. 

In the group behavior class, it keeps custom data structure for each AI present in the game. This data structure is populated as other Actors are registered into this class. Inside the Tick() function, it constantly checks for changes and modify the structure. Changes occurs if an AI is dead or if a resource is completely used up by any character in the game. 


In this project, the main interactable object is the Weapon. Each Character in the game can hold at most two weapons. i.e. one for firing and other as the inventory item which is used once the current weapon is used up completely. Group behavior take place on picking up these weapons. For example, if there is 10 weapons and 5 AIs- each AI can hold 2 weapons. 
If the AI count doubles by keeping weapon count same, each AI takes single weapon. Other cases are discussed below. 

* 5 AI, 6 Weapon – 4 AI hold single weapon and 1 AI hold two.
* 5 AI, 1 Weapon – 1 AI hold the only available weapon. 

Group behavior prevent starvation of resources among AIs. We made separate behavior for Enemy AIs and Support AIs. This prevent group behavior between enemies. Thus, greedy behavior exists between enemies. 

### Combat Behavior

Implementation of Combat behavior is exactly same as group behavior. Every AI spawned is registered into combat behavior. The Behavior class assign a pair of AI to fight. The data structure in this behavior class store an Enemy AI and a Support AI. So, the assigned AIs are given more priority than others. Anyway, this doesn’t mean that the pair thus assigned doesn’t fight with other AIs. Its’ just a priority in case of multiple AIs present in the viewport.
If Assigned is invisible, it goes for the nearest one in front of it. 

AIs see each other AIs by means of Ray casting (Line tracing). 

## Interactive classes

Interactive classes are simply the children of built in AActor class.

```
Actor: An Actor is any object that can be placed into a level. Actors are a generic Class that support 3D transformations such as translation, rotation, and scale. Actors can be created (spawned) and destroyed through gameplay code (C++ or Blueprints). In C++, AActor is the base class of all Actors.

There are several different types of Actors, some examples include: StaticMeshActor, CameraActor, and PlayerStartActor. 

Actors can be thought of, in one sense, as containers that hold special types of Objects called Components. Different types of Components can be used to control how Actors move, how they are rendered, etc. The other main function of Actors is the replication of properties and function calls across the network during play.

Components are associated with their containing Actor when they are created.
```
Most commonly used child class of the Interactable is the weapon class. The name of the class is AInteractable. The class mainly contains a 3d Mesh component and a Collision component. A weapon or a door, Inherits the base class AInteractable contain a Mesh Component and a collision component. 

In weapon class, the mesh is filled with the 3d Mesh of the weapon. 
Whenever the Player or AI overlap the collision component, Type of the object to send to the Player or AI via the interface. Once the data is received by the Character, it decide the action to be performed. If it is a weapon Player decide to pick it up. If it’s a door, an OPEN request is send. 

### Weapon Class

The weapon class AWeapon is the child of the AInteractable class, giving all functionality of the parent. In addition, the weapon class contains lot of weapon-based variables that controls the strength, range and capacity of the weapon. Custom weapon can be made by tweaking the values of Damage, Range and bullet capacity.

### HideSpot Class

The Hide spot class is also the child of AInteractable. The object of this class however doesn’t use the Mesh Component because the object of this class need not be visible in the game. The object of this class is placed around all hiding places in the game. This class is made specifically for the Hiding Behavior of the AIs. AI characters find the required hide spot by the means of Environmental Query System (EQS) present in the Engine. 


```
EQS: The Environment Query System is a feature of the Artificial Intelligence system for collecting data on the environment, asking questions of the data through Tests, then returning then one Item that best fits the questions asked.
```

## Interfaces

Interfaces are like abstract classes in C++. Only members of this classes are a bunch of virtual functions which is implemented in its child classes. 

Interfaces allow different objects to share common functions but allow objects to handle that function differently if it needs to. Any classes that use an interface must implement the functions that are associated with that interface. This gives you a lot of power over your game actors, allowing you to trigger events both in C++ and in blueprints that your game actors can handle differently.
 
For example, consider an interface - TimeBasedBehaviour, which has a function ReactToHighNoon(), and have a bunch of actors respond to this event differently, each with their own behavior.

Flower actors that implement this interface could override the ReactToHighNoon() method to open blossoms completely Frog actors implementing it could override ReactToHighNoon() to hide under rocks, for example

You can then have an event, SunReachedHighNoon() that is triggered which can take any actor, check if it implements the interface, and if it does it can call any of the functions of that interface and the actor will act according to how that specific actor has the behaviors defined. This means you can trigger events anywhere and if you have a pointer to your actor, you can ask it to do specific things without needing to know its types.

In this project, Interfaces are used for numerous tasks, Equipping Weapon, Reloading, Showing-Updating-removing Head-Up-Display (HUD) for Showing Bullet counter and Crosshair. 

Some important interfaces used in this project are listed and briefly explained below:

* Interactable Interface
* Weapon Interface
* HUD Interface

### Interactable Interface

The Interactable Interface is used for the communication between character and pickups like Weapons, Doors etc. Every Interactive object, for example- a weapon or a door, Inherits the base class AInteractable which contain a Mesh Component and a collision component. 

On creating a child class Weapon,the mesh is filled with the 3d Mesh of the weapon. 
Whenever the Player or AI overlap the collision component, Type of the object to send to the Player or AI via the interface. Once the data is received by the Character, it decide the action to be performed. If it is a weapon Player decide to pick it up. If it’s a door, an OPEN request is send. 

### Weapon Interface

Weapon Interface is specifically for weapon. It Contain the messages like, 

```
Reload()
StartFire()
EndFire()
Pickup() 
Putdown()
```
This interface is active if the instance of weapon in the character class is not null. 

Reload() is called by the Character class (either AI or Player character) if user clicked R or bullet count in the weapon is zero. In that case the weapon receives the call and update the bullet count by reducing the magazine count.

StartFire() is called when user click left mouse button and EndFire() is called if button is released. The weapon class get the trigger message and draw Line trace forward to check for any actor and apply damage to the actor if present. Bullet count is also updated alongside. 

Pickup() and Putdown() calls update the physics properties of the weapon for attaching and detaching from the hand bone of the character.  

### HUD Interface

The HUD Interface is for updating the Head On Display for the user to see the status of weapon and other notifications.
The HUD Interface contain following method calls.

```
ActivateHUD()
DeactivateHUD()
RefreshHUD()
```

In this project, the HUD is used for displaying the Health, Bullet counter for weapons and weapon crosshair. The value of the Health changes so Refreshing is required. Refreshing is actually updating variable in the HUD class with that present in the character class. The Health of Player represented using a styled progress bar which is bound by a float variable 
‘health’ RefreshingHUD() simply update the ‘health’ variable and restart the HUD. 

Initial Activation and Deactivation after required period is only needed in some case for some data. Showing the objectives for player to complete doesn’t require constant refreshing. 

## Blueprint Scripting

The Blueprints Visual Scripting system in the Engine is a complete gameplay scripting system based on the concept of using a node-based interface to create gameplay elements from within the Editor. As with many common scripting languages, it is used to define object-oriented (OO) classes or objects in the engine.

This system is extremely flexible and powerful as it provides the ability for designers to use virtually the full range of concepts and tools generally only available to programmers. In addition, Blueprint-specific markup available in Unreal Engine's C++ implementation enables programmers to create baseline systems that can be extended by designers.

The main shortcoming of the blueprint scripting is the performance impact. In Blueprint, the Tick() functions in Actors works almost 10x slower than equivalent C++ code. Normally blueprint is recommended in shorter class with less logical and iterative codes. Loops makes blueprint slow. Programmers often create C++ base class with complex logics in it and calls the implemented C++ functions via blueprint which is faster. 


In this project, we mostly depends on the C++ for more than almost 80% of the total work.
Blueprint is used in Animation blueprints and Behavior trees and simpler tasks. 

## Animation Blueprint

An Animation Blueprint is a specialized Blueprint that controls the animation of a Skeletal Mesh. Graphs are edited inside of the Animation Blueprint Editor, where you can perform animation blending, directly control the bones of a Skeleton, or setup logic that will ultimately define the final animation pose for a Skeletal Mesh to use per frame.

Animation blueprint is a part of every character mesh. It set pose and play animation according to the variables present inside the character mesh. 

Animation blueprint make use of state machines for playing animation. For example, there will be states – Idle, Locomotion and are transition from idle to locomotion is controlled by the attribute- Speed. 

## Behavior Trees

Behavior Tree (BT) is a mathematical model of plan execution used in computer science, robotics, control systems and video games. They describe switchings between a finite set of tasks in a modular fashion. Their strength comes from their ability to create very complex tasks composed of simple tasks, without worrying how the simple tasks are implemented. BTs present some similarities to hierarchical state machines with the key difference that the main building block of a behavior is a task rather than a state.

## AI

In video games, artificial intelligence is used to generate responsive, adaptive or intelligent behaviors primarily in non-player characters (NPCs), similar to human-like intelligence. The techniques used typically draw upon existing methods from the field of artificial intelligence (AI).

The term game AI is used to refer to a broad set of algorithms that also include techniques from control theory, robotics, computer graphics and computer science in general, and so video game AI may often not constitute "true AI" in that such techniques due not facilitate computer learning or other standard criteria, only constituting "automated computation," or a predetermined and limited set of responses to a predetermined and limited set of inputs.

Many industry and corporate voices claim that so-called video game AI has come a long way in the sense that it has revolutionized the way humans interact with all forms of technology, although many expert researchers are skeptical of such claims, and particularly of the notion that such technologies fit the definition of "intelligence" standardly used in the cognitive sciences. Industry voices make the argument that AI has become more versatile in the way we use all technological devices for more than their intended purpose because the AI allows the technology to operate in multiple ways, allegedly developing their own personalities and carrying out complex instructions of the user.

However, many in the field of AI have argued that video game AI is not true intelligence, but an advertising buzzword used to describe computer programs that use simple sorting and matching algorithms to create the illusion of intelligent behavior while bestowing software with a misleading aura of scientific or technological complexity and advancement. Since game AI for NPCs is centered on appearance of intelligence and good gameplay within environment restrictions, its approach is very different from that of traditional AI.

The main Component of AI in the engine are 
* Blackboard
* Behavior Tree
* Environmental Query System

### Black board

A blackboard is a simple place where data can be written and read for decision making purposes. A blackboard can be used by a single AI pawn, shared by a squad, or used for any other purpose where it’s convenient to have a central place to look up relevant data. Blackboards are commonly used with behavior trees, but you could use them separately from behavior trees for convenience or you could make a behavior tree with no need of a blackboard.

These are the reason for using blackboard in AI programming 

1. To Make Efficient Event-Driven Behaviors.
Changes to the blackboard can throw events to change the flow in the behavior tree without having to update every frame to check the value. 

2. To Cache Calculations
Some data must be calculated, which can be a pain to do repeatedly or simply expensive for performance. Therefore, it’s frequently desirable to cache the calculated value for use throughout the behavior tree so it doesn’t have to be repeatedly recalculated.

It’s more performance efficient to calculate the values less frequently, and it’s easier and less error-prone as well. If you recalculate the data in more than one place, you may accidentally calculate it differently (especially if you tweak the calculation after first creating it). Then your entire behavior may suffer from inconsistent values. 

3. As a scratch-pad for behaviors
Some data you may need to use in the behavior tree doesn’t have any other obvious home. For example, when starting a sequence, you may want to note that you’ve started it so that other parts of the behavior tree don’t interrupt it. But that sequence may not readily equate to a specific class or knowledge already stored somewhere. In that case, you may want to use a Context Override* to set and clear the value.

4. To Centralize Data
Blackboards are a nice way to centralize data and remove the need for extra knowledge about where that data comes from. Without blackboards, you often have to go through a number of steps to query information that’s stored in various different classes.

### Behavior Trees

Behavior Tree (BT) is a mathematical model of plan execution used in computer science, robotics, control systems and video games. They describe switchings between a finite set of tasks in a modular fashion. Their strength comes from their ability to create very complex tasks composed of simple tasks, without worrying how the simple tasks are implemented. BTs present some similarities to hierarchical state machines with the key difference that the main building block of a behavior is a task rather than a state.

There are two main types of nodes. Composite Nodes and Leaf Nodes. Composites have children. There is a 3rd type which is a decorator, All the nodes return a value,  if succeeded, failed, or is running are 3 main types. There are two main types of composite nodes, Sequences and Selectors. These are the building blocks of all your decision-making logic. 

Both evaluate their children from left to right, but a Sequence Node will only go on to the next child if the previous one returned a success. The sequence node itself will only return a success if all its children return a success. The Selector node is similar in that it evaluates it's children from left to right, but it will only try the next one if the previous failed. It will try each child in turn seeing if it returns a success. On the first success it finds, it stops evaluating the children and returns a success. 

The leaf nodes are like the leaves on a tree, where the composite nodes are like the branches. There are two main types of leaf nodes. A condition and an Action. You use the Conditions to find out something, like the state of a variable (Is health low? ) and Action performs an action.

### Environmental Query System (EQS)

The Environment Query System is a feature of the Artificial Intelligence system for collecting data on the environment, asking questions of the data through Tests, then returning then one Item that best fits the questions asked.

It can be used to find the closest power up, figure out which enemy is the highest threat, or to find cover.

With EQS, an efficient result can be obtained. The EQS uses two parts. Generators and Tests.
A Generator produces the locations or Actors, referred to as Items, that will be tested and weighted, with the highest weighted location or Actor being returned to the Behavior Tree. Generators can be created in Blueprint and C++.

Tests are how the Environment Query decides which item from the Generator is the "best". The default tests cover a good percentage of use cases, such as can an item trace to another location or is the distance between an item and context is within a specified range.


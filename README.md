# GDDragonBones Plugin
Plugin module for Godot 2.1.4 Game Engine to use DragonBones

## Links
* Homepage Godot: https://godotengine.org
* Homepage DragonBones: http://dragonbones.com

#### Support Versions
* Godot 2.1.4
* DragonBones Pro 5.6

#### Compiling
1. Drop the "GDDragonBones" directory inside the "modules" directory on the Godot source.

2. [See the official docs](http://docs.godotengine.org/en/latest/development/compiling/)
for compilation instructions for every supported platform.

* Compiling Godot Engine with flag for Android: 

```python
android_stl=yes
```

* For iOS not Tested at this moment, maybe some compilation trouble.

* [Godot Engine Src](https://github.com/godotengine/godot)
* [DragonBones common library](https://github.com/DragonBones/DragonBonesCPP)

##
[![Sample](./sample.gif)]()

## Samples

#### Play/Stop animation
```python
var skeleton = get_node("skeleton")

#if needed set speed animation
skeleton.set_speed(0.5)

# start play
skeleton.play("walk")

#stop animation
skeleton.stop()
```

#### Mixing(blending) animation
```python
var skeleton = get_node("skeleton")

#if needed set speed animation
skeleton.set_speed(0.5)

# play animation by layers, see DragonBones documentation for parametrs
skeleton.fade_in("idle", -1, -1, 2, "", GDDragonBones.FadeOut_All)
# "idle" -- name animation
# "-1" -- start delay time (-1 -- no delay)
# "-1" -- loop cnt ([-1] -default count saved in resources, [0] - loop, [0 - N] - cnt loops)
# "2" -- layer for this animation
# "" -- group name
# GDDragonBones.FadeOut_All -- flag for FadeOut animation
#	GDDragonBones.FadeOut_None -- Do not fade out of any animation states.
#	GDDragonBones.FadeOut_SameLayer -- Fade out the animation states of the same layer.
#	GDDragonBones.FadeOut_SameGroup -- Fade out the animation states of the same group.
#	GDDragonBones.FadeOut_SameLayerAndGroup -- Fade out the animation states of the same layer and group.
#	GDDragonBones.FadeOut_All -- Fade out of all animation states.	
#	GDDragonBones.FadeOut_Single  -- Does not replace the animation state with the same name.
#
# skeleton.fade_in("walk", 0.3, 0, 0, "normalGroup", GDDragonBones.FadeOut_All)

skeleton.fade_in("eyes_idle", -1, -1, 1, "", GDDragonBones.FadeOut_SameLayer)

```


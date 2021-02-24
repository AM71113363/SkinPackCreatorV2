<p align="center">
<img align="center" width="456" height="189" src="https://raw.githubusercontent.com/AM71113363/SkinPackCreatorV2/main/info.png">
</p>

# SkinPackCreatorV2
-----

## How To Create .mcpack Skins for Minecraft Bedrock Edition?<br>
1. Run SkinPackCreatorV2.exe (32Bit Windows).<br>
2. Set SkinPack Name.<br>
3. Set SkinPack Description.<br>
4. Set Geometry Options.(You can Select: None, Herois, Custom, CustomSlim).<br>
5. Set Format Options.(You can Select: 128x128, 128x64, 64x64, 64x32).<br>
6. Drag-Drop all Skins;(Multi-Drop)(default: allow only PNG_files with size<5Mb).<br>
7. CLick [ START ].<br>
8. The App will EXIT when DONE.<br>

## Settings<br>
* SkinPack Name can't be black.<br>
* If SkinPack Description is blank the App will use "SkinPack Name" as "SkinPack Description".<br>
* You must select at least one Geometry Option.<br>
* You must select at least one Format Option.<br>
* "Imported Skins NUMBER " show the number of Drag-Drop skins, and not the number of valid skins.<br>
_example_<br>
Drag-Drop 2 PNG files: "steve.png" & "Just_a_random_image.png"<br>
The App will show Imported Skins[ 2 ], but only "steve.png" will be added to .mcpack(because of "Format Options").<br>

## Build.
_for beginners_ <br>
1.Download [DevCpp_v4.9.9.*](http://www.bloodshed.net/) and install it.<br>
2.Run build.bat (it works only with Compiler:  Dev-C++ 4.9.9.*).<br>
_others_ <br>
You already know how to rebuilt it. ^_^<br>
<br>

# NOTE
* Restricted Characters are ENABLED in "SkinPack Name" & "SkinPack Description" input field.<br>
  You can OVERRIDE it by using RightMOuseClick->PASTE (not CTRL+V).<br>
* You can Drag-Drop skins with the same name.<br>
_example_<br>
FOLDER "A" contain skin "steve.png".<br>
FOLDER "B" contain skin "steve.png".<br>
Drag-Drop both skins, The App will add 2 skins(0am.png & 1am.png) with the same SkinDescription in "en_US.lang" file.<br>


### TODO<br>
* Skip: Drag-Drop the same skin more than 1 time.<br>
//currently Drag-Drop the same skin twice, will add 2 png files to .mcpack<br>

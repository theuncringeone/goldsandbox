// Created by Crowbar 0.74

// Stored modelname (without quotes): "valve/models/v_gauss.mdl"
$modelname "v_physics.mdl"
$cd "."
$cdtexture "."
$cliptotextures
$scale 1.0

$bodygroup "body"
{
	studio "v_physics"
}

$flags 0

$origin 2 -4 65

$cbox 0 0 0 0 0 0

$bbox 0 0 0 0 0 0

$hbox 0 "Bip01 L Arm2" 0 -2.8 -1.73 11.68 3.02 4
$hbox 0 "Bip01 L Hand" 0 -0.87 -1.94 3.89 1.03 1.79

$sequence "idle" {
	"anims\idle"
	fps 24
	loop
}
$sequence "deploy" {
	"anims\deploy"
	fps 24
}

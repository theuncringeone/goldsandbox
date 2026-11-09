// Created by Crowbar 0.74

// Stored modelname (without quotes): "valve/models/v_crowbar.mdl"
$modelname "v_chez.mdl"
$cd "."
$cdtexture "."
$cliptotextures
$scale 1.0

$bodygroup "studio"
{
	studio "v_chez"
}

$flags 0


$cbox 0 0 0 0 0 0

$bbox 0 0 0 0 0 0

$hbox 0 "Bip01 R Arm2" 0 -2.12 -1.98 11.368422 2.1 2.18
$hbox 0 "Bip01 R Hand" 0 -0.87 -5.93 7.85 2.94 19.950001

$sequence "deploy" {
	"anims/deploy"
	fps 24
}
$sequence "idle" {
	"anims/idle"
	fps 24
}
$sequence "eat" {
	"anims/eat"
	fps 24
}
$sequence "throw" {
	"anims/throw"
	fps 24
}


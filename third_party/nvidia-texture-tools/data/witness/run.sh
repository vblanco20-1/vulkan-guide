
FILES=(hallway windmill tunnel theater tower hub mine archway hut shaft)
EXT=dds
#FILES=(kodim01 kodim02 kodim03 kodim04 kodim05 kodim06 kodim07 kodim08)
#EXT=png


for file in "${FILES[@]}"
do
	#echo $file
	
	# Output histogram
	#./nvdecompress -histogram $file.$EXT

	# R11G11B10_FLOAT
	#./nvcompress -silent -alpha -nomips -dds10 -rgb $file.$EXT $file.r11g11b10.dds
	#./nvimgdiff -alpha $file.$EXT $file.r11g11b10.dds

	# RGBM
	#./nvcompress -silent -alpha -nomips -rgb -rgbm $file.$EXT $file.rgbm.dds
	#./nvimgdiff -alpha $file.$EXT $file.rgbm.dds

	# RGB-DXT1
	#./nvcompress -silent -alpha -nomips -bc1 $file.$EXT $file.bc1.dds
	#./nvimgdiff -alpha $file.$EXT $file.bc1.dds

    # RGBM-DXT5 Naive
	#./nvcompress -silent -alpha -nomips -bc3 -rgbm $file.$EXT $file.bc3-rgbm.dds
	#./nvimgdiff -alpha $file.$EXT $file.bc3-rgbm.dds

	# RGBM-DXT5 Optimized
	./nvcompress -silent -alpha -nomips -bc3_rgbm $file.$EXT $file.bc3-rgbm.dds
	./nvimgdiff -alpha $file.$EXT $file.bc3-rgbm.dds

	# BC6
	#./nvcompress -silent -alpha -nomips -bc6 $file.$EXT $file.bc6.dds
	#./nvimgdiff -alpha $file.$EXT $file.bc6.dds

    # ETC2-EAC
    ./nvcompress -silent -alpha -nomips -etc_rgbm
done

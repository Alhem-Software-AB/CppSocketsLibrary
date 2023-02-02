#!/bin/sh
PREV=empty-file.txt
CHANGELOG=../Changelog
rm -f $CHANGELOG
touch $CHANGELOG
git add $CHANGELOG
for i in `cat files.txt`; do
	VERSION=`echo ${i} | awk -F \- '{print $2}' | sed -e 's/.tar.gz//g'`
	OUT=`echo ${i} | sed -e 's/.tar.gz/.txt/g'`
	TAG=`echo $VERSION | sed -e 's/\./_/g'`
	echo VERSION=$VERSION,  FILENAME=${i},  TAG=$TAG
#	tar tzf ${i} | sort > $OUT
	mkdir -p $VERSION
	# -------
	cd $VERSION
	rm -rf *
	tar xzf ../${i}
	if [ -d Sockets-$VERSION ]; then
#		echo Have SUBDIR - removed
		mv Sockets-$VERSION/* .
		rmdir Sockets-$VERSION
	fi
	if [ -f mkdot.sh ]; then
		rm -f mkdot.sh
	fi
	if [ -f OSX.zip ]; then
		rm -f OSX.zip
	fi
	for f in `ls -F | grep /`; do
		rm -rf ${f}
	done
	ls * > ../$OUT
	cd ..
	# -------
#	rm -f ../src/*
	cp $VERSION/* ../src 2>/dev/null
	echo Version $VERSION >> $CHANGELOG
	echo -------------------------- >> $CHANGELOG
	diff $PREV $OUT >> $CHANGELOG
	echo >> $CHANGELOG
	#
	for j in `diff $PREV $OUT | grep \< | sed -e 's/< //g'`; do
		echo Removed: ${j}
		git rm ../src/${j}
	done
	for j in `diff $PREV $OUT | grep \> | sed -e 's/> //g'`; do
		echo Added: ${j}
		git add ../src/${j}
	done
	# $ git tag -a v1.4 -m "my version 1.4"
	git commit -am "add version $VERSION"
	git push origin main
	git tag -a v$VERSION -m "${i}"
	PREV=$OUT
done

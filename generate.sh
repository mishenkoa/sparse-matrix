BASEDIR=$PWD/$(dirname "$0")
rm -r $BASEDIR/debug
rm -r $BASEDIR/release
mkdir -p $BASEDIR/debug
mkdir -p $BASEDIR/release

cd $BASEDIR/debug
cmake .. -DCMAKE_BUILD_TYPE=Debug

cd $BASEDIR/release
cmake .. -DCMAKE_BUILD_TYPE=Release
Welcome to the Volume Image Processing and Rendering Engine (VIPRE).

For information about the project, updates, and in-depth documentation
about compiling and running the examples, please refer to the INSTALL.txt
document or the VIPRE wiki on Github:

	https://github.com/cnoon/VIPRE - this is not public...yet ;-)

The rest of this document is broken down as follows:

	1. A short tutorial of how to build VIPRE
	2. An overview of the vipre library
	3. An overview of the vipreDICOM library

Christian Noon
Project Lead
Dec 14th, 2011

--

Section 1. How to build VIPRE
==========================================================================

VIPRE uses the CMake build system to make cross-platform compiling as
easy as possible for users. The CMakeLists.txt files you will find in
most of the VIPRE directories contain specific information for compiling
on many different platforms.

If you don't already have CMake install on your system, you can download
it from:

	http://www.cmake.org

Please note that to compile VIPRE, you must have CMake 2.8.0 or greater
installed. Otherwise certain portions of the CMakeLists.txt files are not
supported.

For generating an "in-source" build, you can use the configure script
provided in the VIPRE installation directory. This runs the cmake command
line tool provided by the CMake installation. The configure script is
very short running the command "cmake . -DCMAKE_BUILD_TYPE=Release" to
ensure you get the best performance and smallest size for your VIPRE
libraries and example applications.

	cd VIPRE
	./configure
	if Unix
		make
		sudo make install
	if Mac with Xcode
		Open the VIPRE.xcodeproj file with Xcode
		Switch the build to "install"
		Build
	if Windows with Visual Studio
		Open the ALL_BUILD.vcproj file with Visual Studio
		Go to Build->Batch Build
		Check INSTALL
		Hit the Build button

For generating an "out-of-source" build, you can still use the configure
script, but first need to make an out-of-source build directory. This
is useful for keeping all the CMake build system files out of the
original VIPRE library source.

	cd VIPRE
	mkdir build.myOS
	cd build.myOS
	../configure
	if Unix
		make
		sudo make install
	if Mac with Xcode
		Open the VIPRE.xcodeproj file with Xcode
		Switch the build to "install"
		Build
	if Windows with Visual Studio
		Open the ALL_BUILD.vcproj file with Visual Studio
		Go to Build->Batch Build
		Check INSTALL
		Hit the Build button

Section 2. Overview of the vipre library
==========================================================================

The core vipre library provides various volume rendering techinques
(Raycasting, Orthogonal Texture Slicing, etc.) as well as all the support
level classes such ClippingPlanes, BoundingBoxes, VolumeSlices, Shaders,
etc.

Section 3. Overview of the vipreDICOM library
==========================================================================

The vipreDICOM library can extract header and voxel data from DICOM files.
It supports DICOM files with 1, 3, and 4 channels of data as well as
signed/unsigned int8, int16, and int32 data types. It can also normalize
all the voxel data to any of the supported data types. Once the voxel data
has been extracted, it can be passed off to the vipre library to be
rendered.

There are two main purposes for the library. The first is to make DICOM
data extraction easy. The second purpose is to make it easy to render the
DICOM voxel data. Therefore, the vipreDICOM library acts as an abstraction
layer between DCMTK and OSG. Since both libraries do things differently,
vipreDICOM integrates the two together at the voxel data level so you, the
developer, won't have to deal with these issues.

There are several main classes which you will generally use to extract
DICOM information:

vipreDICOM::Slice - The slice class is a container for all the DICOM
header info and voxel data for a single DICOM image. It makes extensive
use of DCMTK to do the data extraction.

vipreDICOM::Series - The series class is a container for all slice header
info and voxel data for a set of equally spaced slices. It uses a series
( no pun intended :P ) of checks to ensure the voxel data is valid.

vipreDICOM::SeriesBuilder - The series build class takes a list of files
or directories and generats a list of unique series objects. It can do
this very quickly by using only the SeriesID of each valid DICOM file. If
you want to ensure each returned Series object is valid, then enable the
validateSeries flag for each public method.

The other classes in the library are more support for the three main
classes. Generally you should not have to use these directly. The most
common use will be to use the SeriesBuilder to generate a validate
Series. Then the Series object will be passed off to the vipre library
for rendering.

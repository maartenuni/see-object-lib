#!/usr/bin/env python3

# This file is part of see-object.
#
# see-object is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# see-object is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with see-object.  If not, see <http://www.gnu.org/licenses/>.


'''
This small python utility is create to create boiler plate header and source
file code. The default license etc is generated. The headers are created using
c calling conventions. And the source files will automagically include the
header.
It is boiler plate, hence it is created in order to assist the programmer.
However, it is ultimately the responsibility of the programmer to make sure
that the code is correct!
'''
# TODO automagically update the CMakeList.txt files.


# system modules
import argparse as ap
import os.path as path
import sys
from sys import argv

# Custom modules
import file_contents

# Constants

# The root folder of see-object
ROOT_DIR = path.abspath(path.dirname(argv[0]) + "/../")

# Message for when the comment exists.
FILE_EXIST_MSG = "The file {} seems to exists cowardly refusing to overwrite."

def write_header(
        filename,
        classname,
        parentname,
        project,
        namespace,
        force=False,
        lic_only=False
    ):
    """Writes the file header."""
    if path.exists(filename) and not force:
        print(FILE_EXIST_MSG.format(filename), file=sys.stderr)
        return
    with open(filename, "w") as f:
        content = None
        if not lic_only:
            content = file_contents.header_content(
                classname,
                parentname,
                namespace=namespace,
                project=project
                )
        else:
            content = file_contents.LIC_STR.format(PROJECT=project)

        f.write(content)


def write_cfile(
        filename,
        classname,
        parentname,
        project,
        namespace,
        force=False,
        lic_only=False
    ):
    """Writes the class implementation file"""
    if path.exists(filename) and not force:
        print(
            FILE_EXIST_MSG.format(filename),
            file=sys.stderr
            )
        return
    with open(filename, "w") as f:
        if not lic_only:
            f.write(
                file_contents.implementation_content(
                    classname,
                    parentname,
                    namespace,
                    project
                    )
            )
        else:
            f.write(file_contents.LIC_STR.format(PROJECT=project))


# Parsing commandline
parser = ap.ArgumentParser(
    description=globals()["__doc__"],
    epilog="Enjoy creating See Objects"
    )
parser.add_argument(
    "classname",
    help=(
        "The ClassName to create without extension, use CamelCase for "
        "readability, and don't append the namespace. e.g. Object not SeeObject"
        )
    )
parser.add_argument(
    "-p",
    "--parentname",
    default="Object",
    help="The name of the parent class."
    )
parser.add_argument(
    "-s",
    "--source",
    help="Directory to place the new file(s) in",
    default=ROOT_DIR + "/src/"
    )
parser.add_argument(
    "-P",
    "--project",
    help="The name for the project to enter in the license.",
    default="see-object"
    )
parser.add_argument(
    "-n",
    "--namespace",
    help="A the name of the namespace preferably capitalized eg: See",
    default="See"
    )
parser.add_argument(
    "-H",
    "--header",
    help="Write header file only.",
    action='store_true'
    )
parser.add_argument(
    "-c",
    "--object",
    help="Write object/c file only.",
    action='store_true'
    )
parser.add_argument(
    "-f",
    "--force",
    help="Overwrite existing classes",
    action='store_true'
    )
parser.add_argument(
    "-l",
    "--licence-only",
    help="Only write the license header, purposefully omit the boiler plate code",
    action='store_true'
    )
args = parser.parse_args()

# Set constants for the remainder
SRC_DIR = args.source
CLASSNAME = args.classname
PARENTNAME = args.parentname
PROJECTNAME = args.project
NAMESPACENAME = args.namespace
FILENAME_HDR = SRC_DIR + args.classname + ".h"
FILENAME_C = SRC_DIR + args.classname + ".c"
FORCE = args.force
LICENCE_ONLY = args.licence_only

# Write files
if args.header:     # Write the header only
    write_header(
        FILENAME_HDR,
        CLASSNAME,
        PARENTNAME,
        PROJECTNAME,
        NAMESPACENAME,
        FORCE,
        LICENCE_ONLY
    )
elif args.object:   # Write the c file only
    write_cfile(
        FILENAME_C,
        CLASSNAME,
        PARENTNAME,
        PROJECTNAME,
        NAMESPACENAME,
        FORCE,
        LICENCE_ONLY
    )
else:               # write both
    write_header(
        FILENAME_HDR,
        CLASSNAME,
        PARENTNAME,
        PROJECTNAME,
        NAMESPACENAME,
        FORCE,
        LICENCE_ONLY
    )
    write_cfile(
        FILENAME_C,
        CLASSNAME,
        PARENTNAME,
        PROJECTNAME,
        NAMESPACENAME,
        FORCE,
        LICENCE_ONLY
    )

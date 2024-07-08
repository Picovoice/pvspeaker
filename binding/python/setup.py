#
# Copyright 2024 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#

import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvspeaker')
os.mkdir(package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '__init__.py'), os.path.join(package_folder, '__init__.py'))
shutil.copy(os.path.join(os.path.dirname(__file__), '_pvspeaker.py'), os.path.join(package_folder, '_pvspeaker.py'))

shutil.copytree(
    os.path.join(os.path.dirname(__file__), '../../resources/scripts'),
    os.path.join(package_folder, 'resources/scripts'))

platforms = ('linux', 'mac', 'raspberry-pi', 'windows')

os.mkdir(os.path.join(package_folder, 'lib'))
for platform in platforms:
    shutil.copytree(
        os.path.join(os.path.dirname(__file__), '../../lib', platform),
        os.path.join(package_folder, 'lib', platform))

MANIFEST_IN = """
include pvspeaker/LICENSE
include pvspeaker/__init__.py
include pvspeaker/_pv_speaker.py
include pvspeaker/lib/linux/x86_64/libpv_speaker.so
include pvspeaker/lib/mac/x86_64/libpv_speaker.dylib
include pvspeaker/lib/mac/arm64/libpv_speaker.dylib
recursive-include pvspeaker/lib/raspberry-pi *
include pvspeaker/lib/windows/amd64/libpv_speaker.dll
recursive-include pvspeaker/resources/scripts *
"""

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write(MANIFEST_IN.strip('\n '))

with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r') as f:
    long_description = f.read()

setuptools.setup(
    name="pvspeaker",
    version="1.0.0",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="Speaker library for Picovoice.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/pvspeaker",
    packages=["pvspeaker"],
    install_requires=[],
    include_package_data=True,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Topic :: Multimedia :: Sound/Audio :: Speech"
    ],
    python_requires='>=3.8',
    keywords="Audio Player",
)

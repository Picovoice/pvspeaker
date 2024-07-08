import os
import shutil

import setuptools

os.system('git clean -dfx')

package_folder = os.path.join(os.path.dirname(__file__), 'pvspeakerdemo')
os.mkdir(package_folder)

shutil.copy(os.path.join(os.path.dirname(__file__), '../../LICENSE'), package_folder)

shutil.copy(
    os.path.join(os.path.dirname(__file__), 'pv_speaker_demo.py'),
    os.path.join(package_folder, 'pv_speaker_demo.py'))

with open(os.path.join(os.path.dirname(__file__), 'MANIFEST.in'), 'w') as f:
    f.write('include pvspeakerdemo/LICENSE\n')
    f.write('include pvspeakerdemo/pv_speaker_demo.py\n')

with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r') as f:
    long_description = f.read()

setuptools.setup(
    name="pvspeakerdemo",
    version="1.0.0",
    author="Picovoice",
    author_email="hello@picovoice.ai",
    description="Speaker library for Picovoice.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Picovoice/pvspeaker",
    packages=["pvspeakerdemo"],
    install_requires=["pvspeaker==1.0.0"],
    include_package_data=True,
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Topic :: Multimedia :: Sound/Audio :: Speech"
    ],
    entry_points=dict(
        console_scripts=[
            'pv_speaker_demo=pvspeakerdemo.pv_speaker_demo:main',
        ],
    ),
    python_requires='>=3.8',
    keywords="Audio Player",
)

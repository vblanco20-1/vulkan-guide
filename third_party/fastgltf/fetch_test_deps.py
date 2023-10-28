#!/usr/bin/env python3

import os
import shutil
import sys
import urllib.error
import urllib.request
import zipfile

example_deps_urls = {
    'glfw': "https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip",
    'glm': "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip",
    'stb': "https://github.com/nothings/stb/archive/refs/heads/master.zip",
    'glad': "https://github.com/Dav1dde/glad/archive/refs/heads/glad2.zip",
}
test_deps_urls = {
    'catch2': "https://github.com/catchorg/Catch2/archive/refs/tags/v3.3.2.zip",
    'corrosion': "https://github.com/corrosion-rs/corrosion/archive/refs/heads/master.zip",
}
deps_folder = "deps/"


def download_zip_and_extract(url, output_folder, name):
    output = f'{output_folder}{name}'
    file_path, _ = urllib.request.urlretrieve(url, f'{output}.zip')
    with zipfile.ZipFile(file_path, "r") as zip_ref:
        names = zip_ref.namelist()
        if len(names) == 0:
            return
        # Remove any old versions of the downloaded dependency
        if os.path.isdir(output):
            shutil.rmtree(output)

        zip_ref.extractall(output_folder)
        # The zip file contains another folder called the same name.
        if name.lower() in names[0].lower():
            os.rename(f'{output_folder}{names[0]}', output)

    os.remove(file_path)


def main():
    for name, url in example_deps_urls.items():
        try:
            download_zip_and_extract(url, deps_folder, name)
            print(f'Finished downloading {name}')
        except urllib.error.HTTPError:
            print(f'Could not download {url}.', file=sys.stderr)
            break

    for name, url in test_deps_urls.items():
        try:
            download_zip_and_extract(url, deps_folder, name)
            print(f'Finished downloading {name}')
        except urllib.error.HTTPError:
            print(f'Could not download {url}.', file=sys.stderr)


if __name__ == '__main__':
    main()

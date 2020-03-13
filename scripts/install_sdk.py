#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import sys
import traceback
import logging
import shutil
import tarfile


def extract_package(filename, outdir):
    logging.info("extract sdk %s to %s ..." % (filename, outdir))
    if os.path.exists(outdir):
        shutil.rmtree(outdir)
    os.makedirs(outdir)
    #os.system("tar xvf %s -C %s" % (filename, outdir))
    with tarfile.open(filename, 'r') as tar:
        tar.extractall(path=outdir)

    # 确定SDK目录，通过查找 build_app.sh 所在目录，只查找两级目录
    sdk_dir = ''
    for file in os.listdir(outdir):
        if file == 'build_app.sh':
            sdk_dir = outdir
            break
        subdir = os.path.join(outdir, file)
        for subfile in os.listdir(subdir):
            if subfile == 'build_app.sh':
                sdk_dir = subdir
                break

    # 返回SDK目录(build_app.sh所在目录)
    logging.info("sdk extracted to %s!", sdk_dir)
    return sdk_dir

def install_sdk(src_dir, dst_dir):
    move_file(src_dir, dst_dir, "demos")
    move_file(src_dir, dst_dir, "sdk")
    move_file(src_dir, dst_dir, "platforms")
    move_file(src_dir, dst_dir, "build_app.sh")
    logging.info("sdk installed to %s!", dst_dir)

def move_file(src_dir, dst_dir, file):
    src_file = os.path.join(src_dir, file)
    dst_file = os.path.join(dst_dir, file)
    if os.path.exists(dst_file):
        if os.path.isfile(dst_file):
            os.remove(dst_file)
        else:
            shutil.rmtree(dst_file)
    shutil.move(src_file, dst_file)


def main():
    logging.basicConfig(level=logging.INFO)

    sdk_package = sys.argv[1]
    install_dir = sys.argv[2]
    tmp_dir = os.path.join(sys.argv[3], "sdk_tmp")

    try:
        sdk_dir = extract_package(sdk_package, tmp_dir)
        if sdk_dir == '':
            logging.error('extract sdk package failed!')
            return

        install_sdk(sdk_dir, install_dir)

    except Exception as e:
        traceback.print_exc()
        logging.error(str(e))
        exit(1)

    exit(0)

if __name__ == '__main__':
    main()


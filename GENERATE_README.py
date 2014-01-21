#-*- coding:utf-8 -*-
import os
import os.path
import codecs
import urllib

PARSE_URL_DIC = dict(BAE="http://chinapub.duapp.com/gen_md?src=",
                 GAE="",
                 SAE="")
GITHUB_BASE_URL = "https://raw2.github.com/zyxstar/markdown_note/master/docs"

def build_one_file(folders,filename):
    _note_url = "%s/%s/%s" % (GITHUB_BASE_URL, "/".join([urllib.quote_plus(_fo.encode('utf-8')) for _fo in folders]), urllib.quote_plus(filename.encode('utf-8')))

    _parse_lines = []
    for _k,_v in PARSE_URL_DIC.items():
        if not _v =="":
            _parse_lines.append("[View On %s](%s%s)" % (_k, _v, urllib.quote_plus(_note_url)))


    return "- %s %s" % (" ".join(_parse_lines),filename)


def yield_subfiles():
    _docs_dir = os.path.join(os.path.dirname(__file__), './docs/')
    def _find_subfiles(*folders):
        _folder_path = os.path.join(_docs_dir, '/'.join(folders))
        for _dirpath, _dirnames, _filenames in os.walk(_folder_path):
            for _f in sorted(_filenames):
                yield map(lambda _fo:_fo.decode('gbk'),folders), _f.decode('gbk')
            for _d in sorted(_dirnames):
                _folders = list(folders)
                _folders.append(_d)
                for _a, _b in _find_subfiles(*_folders):
                    yield _a, _b
            break
    return _find_subfiles()


def build_subfiles():
    _marks = []
    _lines = ["Note Index","=========="]
    for _folders,_filename in yield_subfiles():
        if len(_folders) and not _folders[-1] in _marks:
            _lines.append("\n%s %s" % ("#"*(len(_folders)+1), _folders[-1]))
            _marks.append(_folders[-1])
        _lines.append(build_one_file(_folders,_filename))
    return _lines


def write_readme():
    with codecs.open('README.md', 'w', 'utf-8') as _f:
        _f.writelines('\r\n'.join(build_subfiles()))


if __name__ == '__main__':
    write_readme()



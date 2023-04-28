# -*- coding: utf-8 -*-

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information


import os

from datetime import datetime


DOXYGEN_XML_PATH = os.path.join((os.path.dirname(os.path.abspath(__file__))), "doxygen_out/xml")


project = "Gaia Firmware"
copyright = f"{datetime.now().year}, Gaia"
author = "Gaia"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.imgmath",
    "sphinx.ext.todo",
    "breathe",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]
source_suffix = ".rst"
master_doc = "index"


breathe_projects = {"gaiafirmware": f"{DOXYGEN_XML_PATH}"}
breathe_default_project = "gaiafirmware"



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_favicon = "favicon.png"
html_logo = "favicon.png"

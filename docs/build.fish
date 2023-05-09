
function print_label -d "Print out text on a white background and black text"
    set_color -b brwhite
    set_color -o black
    echo $argv
    set_color normal
end

cd (dirname (status --current-filename))/..
print_label "Sourcing VENV (dont't forget to deactivate to disable VENV)"
source venv/bin/activate.fish
print_label "Generating doxyfile"
doxygen Doxyfile
cd docs
print_label "Generating HTML"
make html
print_label "Starting web server on 127.0.0.1:8000"
python3 -m http.server -b 127.0.0.1 -d _build/html/ 8000

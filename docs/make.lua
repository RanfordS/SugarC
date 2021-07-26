skips = 'echo "'..("\n"):rep(20)..'" | '
os.execute (skips.."pdflatex tokenizer -interaction=nonstpmode -draftmode > /dev/null")
os.execute "rail tokenizer"
os.execute (skips.."pdflatex tokenizer -interaction=nonstpmode > /dev/null")

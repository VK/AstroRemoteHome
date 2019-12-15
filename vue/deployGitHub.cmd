cd dist
git init
git checkout -b  gh-pages
git remote add origin https://github.com/VK/AstroRemoteHome.git
git add .
git commit -a -m "initial commit"
git push --force origin gh-pages
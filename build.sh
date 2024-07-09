if [ $1 == "lib" ]; then
  gcc -o libqoir.so -fpic -shared src/img_dec.c src/img_enc.c
  echo "qoi-r compiled successfully (libqoir.so)"
  exit 0
fi

if [ $1 == "render" ]; then
  gcc render/render.c -o render/render -lqoir -L. -lSDL2 -lSDL2_image
  echo "renderer compiled successfully (render)"
  exit 0
fi

if [ $1 == "qoibuild" ]
  gcc build.c -o build -lqoir -L..
  echo "qoi builder compiled successfully (build)"
fi

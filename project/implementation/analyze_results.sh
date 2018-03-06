if [[ ! -e "$1" ]]; then
  echo "file $1 doesn't exist"
  exit 1
fi

cat $1 | pilot analyze -i 1 - > $(ls $1 | cut -d '.' -f 1).csv

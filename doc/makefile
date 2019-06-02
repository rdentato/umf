##  (C) Remo Dentato (rdentato@gmail.com)
##  UMF is distributed under the terms of the MIT License
##  as detailed in the 'LICENCE' file.

umf.odt: umf.md umf-ref.odt
	pandoc -o umf.odt -t odt --reference-doc=umf-ref.odt umf.md

clean:
	rm -f umf.odt umf.htm
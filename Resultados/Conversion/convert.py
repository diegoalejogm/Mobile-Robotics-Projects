
with open('./PrimeraMuestraSceneFrame.map') as f:
  content = f.readlines()
  with open('./PrimeraMuestraSceneFrame2.map', 'w') as f2:
  	first = True;
  	for line in content:
  		l = line.split(' ');
  		f2.write(str(int(l[0])+450)+" "+l[1]+"\n");

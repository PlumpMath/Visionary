
import Blender, meshtools
from Blender import Types, Object, NMesh, Camera, Lamp, Scene, Mathutils
from Blender.Scene import Render
from Blender.Scene.Render import *
import warnings
import struct, cStringIO, time, operator, copy, math
import string, chunk, os

# Filter warnings for crc data structure
warnings.filterwarnings( "ignore", "hex/oct constants > sys.maxint will return positive values in Python 2.4 and up" )

ObjectInstances = {}

def main():
	
	print "-------------------------------------------------------"
	print "                      start (Practice.py)\n"
	
	start = time.clock()
	
	# Try getting the names of all of the selected items in the scene
	objects	= Blender.Object.GetSelected()
	if not objects:
		meshtools.print_boxed("No objects are selected.")
		return
	
	objects.sort(lambda a,b: cmp(a.name,b.name))	
		
	def NumUsers( o ):
		try: return o.users
		except: return 0
		
	global ObjectInstances
	
	# walk through list of selected objects and check
	for o in objects:
		print "%s, %s:\n\tdata: %s, %s, %s, [Users %s]"  % ( o.name, type(o), o.data, o.getData(True), type(o.data), NumUsers(o.data) )
		# o.getBoundBox(), o.getEuler(), o.getLocation(), o.getSize()

		if not o.data.name in ObjectInstances:
			ObjectInstances[o.data.name] = o.data # this will be the same so just add it?
		else:
			numUsers = NumUsers(o.data)
			if numUsers > 1:
				print "Found %s" % o.data.name
			elif numUsers:
				print "Error: %d, was used only once but found on list possible duplicate name!" % (o.data.name)
		
		# Create a list of verts by Material!
		if type(o.data) == Types.NMeshType:
			faceByMaterial = [[] for i in range(len(o.data.materials))]
	
			# create face listing by material!
			for face in o.data.faces: 
				# print "material", face.mat
				faceByMaterial[face.mat].append(face)
				
			for (indx, faces) in enumerate(faceByMaterial):
				# print indx, o.data.materials[indx], len(faces)
				for face in faces: print face,	
				# print
			
	end = time.clock()
	
	print
	
	# Print status message
	seconds = " in %.2f %s" % (end-start, "seconds")
	message = "Completed operation in " + seconds
	meshtools.print_boxed( message )
	
main()
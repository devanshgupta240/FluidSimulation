




fluidsimulation: mainSet.o SceneSet.o CameraSet.o RenderObjSet.o ObjectFactorySet.o SimObjSet.o RenderFluidSet.o SimFluidSet.o SimulationSet.o InputsSet.o
	g++ $(CFLAGS) -Wall -o fluidsimulation mainSet.o SceneSet.o CameraSet.o RenderObjSet.o ObjectFactorySet.o SimObjSet.o RenderFluidSet.o SimFluidSet.o SimulationSet.o InputsSet.o -I/usr/include -L/usr/lib -lglut -lGL -lGLU -lX11


SceneSet.o: Scene.cpp Scene.h
	g++ $(CFLAGS) -Wall -c -w Scene.cpp -o SceneSet.o

mainSet.o: main.cpp
	g++ $(CFLAGS) -Wall -c main.cpp -o mainSet.o

RenderObjSet.o: RenderObj.cpp RenderObj.h
	g++ $(CFLAGS) -Wall -c RenderObj.cpp -o RenderObjSet.o

ObjectFactorySet.o: ObjectFactory.cpp ObjectFactory.h
	g++ $(CFLAGS) -Wall -c ObjectFactory.cpp -o ObjectFactorySet.o

SimObjSet.o: SimObj.cpp SimObj.h
	g++ $(CFLAGS) -Wall -c SimObj.cpp -o SimObjSet.o

RenderFluidSet.o: RenderFluid.cpp RenderFluid.h
	g++ $(CFLAGS) -Wall -c RenderFluid.cpp -o RenderFluidSet.o

SimFluidSet.o: SimFluid.cpp SimFluid.h
	g++ $(CFLAGS) -Wall -c SimFluid.cpp -o SimFluidSet.o

SimulationSet.o: Simulation.cpp Simulation.h
	g++ $(CFLAGS) -Wall -c -w Simulation.cpp -o SimulationSet.o

InputsSet.o: Inputs.cpp Inputs.h
	g++ $(CFLAGS) -Wall -c Inputs.cpp -o InputsSet.o

CameraSet.o: Camera.cpp Camera.h
	g++ $(CFLAGS) -Wall -c Camera.cpp -o CameraSet.o

clean:
	rm -f *.o fluidsimulation
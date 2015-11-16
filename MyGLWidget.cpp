#include <GL/glew.h>
#include "MyGLWidget.h"
#include "model.h"
#include <iostream>
#include <math.h>
using namespace std;


MyGLWidget::MyGLWidget (QGLFormat &f, QWidget* parent) : QGLWidget(f, parent)
{
  setFocusPolicy(Qt::ClickFocus); // per rebre events de teclat
  scale = 1.0f;
  h=1.0;
  xClick = yClick = 0;
  angleY = 0.0;
  angleX = 0.0;
  DoingInteractive = NONE;
  RadioSphere=2*sqrt(2);
  
}

void MyGLWidget::calculo_caja(Model *m, glm::vec3 *centreModAux, float *scale){
  int i=0;
  
  double maxx=m->vertices()[i];
  double minx=m->vertices()[i];
  double maxy=m->vertices()[i+1];
  double miny=m->vertices()[i+1];
  double maxz=m->vertices()[i+2];
  double minz=m->vertices()[i+2];
  i=i+3;
    
  while(i<m->vertices().size()){
    //const double *x=&m.vertices()[i];
    //const double *y=&m.vertices()[i+1];
    //const double *z=&m.vertices()[i+2];
    if(maxx<m->vertices()[i])maxx=m->vertices()[i];
    if(minx>m->vertices()[i])minx=m->vertices()[i];
    if(maxy<m->vertices()[i+1])maxy=m->vertices()[i+1];
    if(miny>m->vertices()[i+1])miny=m->vertices()[i+1];
    if(maxz<m->vertices()[i+2])maxz=m->vertices()[i+2];
    if(minz>m->vertices()[i+2])minz=m->vertices()[i+2];
    i=i+3;
  }
  /*centreModAux[0]=(minx+maxx)/2.0;
  centreModAux[1]=(miny+maxy)/2.0;
  centreModAux[2]=(minz+maxz)/2.0;*/
  *centreModAux = glm::vec3((minx+maxx)/2.0,miny,(minz+maxz)/2.0);
  /*if(RadioSphere<max(maxx-minx,max(maxy-miny,maxz-minz)))
    RadioSphere=max(maxx-minx,max(maxy-miny,maxz-minz));
    */
  *scale=h/(maxy-miny);
  //RadioSphere=max(maxx-minx,max(maxy-miny,maxz-minz));
  
    printf("%f\n",RadioSphere);
    printf("%f\n",maxx);
    printf("%f\n",maxy);
    printf("%f\n",maxz);
    printf("%f\n",minx);
    printf("%f\n",miny);
    printf("%f\n",minz);
    distObs=RadioSphere*2;
    

    FOV=glm::degrees(glm::atan(((float)2.0*(float)RadioSphere)/(((float)distObs-(float)RadioSphere)*2)));
    //FOV=((float)M_PI)/2.0f;
    FOVC=FOV;
} 

void MyGLWidget::projectTransform()
{

  /*//glm::perspective(FOV en radians, ra window, znear, zfar);
  
  //FOV = 2*glm::atan(glm::tan(FOV)/ra);
  if (ra<1){
    
      FOV = glm::degrees(glm::atan(glm::tan(glm::radians(anguloIni))/ra));
      printf("Ya entra, angulo : %f y ra: %f\n",FOV,ra);
   
    
  }
  glm::mat4 Proj = (glm::mat4)glm::perspective(glm::radians(2*FOV),ra,distObs-RadioSphere,distObs+RadioSphere);
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);*/
    double ra = double(width())/double(height());
    if (ra<1){   
      FOVC = glm::degrees(glm::atan(glm::tan(FOV)/ra));
      printf("Ya entra, angulo : %f y ra: %f\n",FOVC,ra);    
    }
    glm::mat4 Proj = glm::perspective(FOVC,(float)ra,(float)distObs-(float)RadioSphere,(float)distObs+(float)RadioSphere);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform()
{
    
    //glm::mat4 View = glm::lookAt(glm::vec3(0.0,0.0,distObs),glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,1.0,0.0));
    glm::mat4 View = glm::mat4(1.);
    View = glm::translate(View,glm::vec3(0.,0.,-distObs));
    View = glm::rotate(View,(float)0.,glm::vec3(0.,0.,1.));
    View = glm::rotate(View,angleX,glm::vec3(1.,0.,0.));
    View = glm::rotate(View,angleY,glm::vec3(0.,1.,0.));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::carregaModel()
{
    
    m.load("./models/Patricio.obj");
    calculo_caja(&m,&centreMod,&scale1);

}

void MyGLWidget::initializeGL () 
{
  // glew és necessari per cridar funcions de les darreres versions d'OpenGL
  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError();  // Reinicia la variable d'error d'OpenGL
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaModel();
  carregaShaders();
  createBuffers();
  projectTransform();
  viewTransform();
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  // Activem el VAO per a pintar la caseta 
  //glBindVertexArray (VAO_Casa);

  // pintem
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

  // Activem el VAO per a pintar la caseta 
  glBindVertexArray (VAO_Model);

  // pintem
  modelTransform();
  glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);

  // Activem el VAO per a pintar la caseta 
  glBindVertexArray (VAO_Model2);

  // pintem
  modelTransform2();
  glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);


  
  // Activem el VAO per a pintar el terra 
  glBindVertexArray (VAO_Terra);
  terraTransform();
  // pintem
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.,0.,-1.));
  transform = glm::scale(transform, glm::vec3(scale1));
  transform = glm::translate(transform,-centreMod);
  //transform = glm::rotate(transform, .58f, glm::vec3(1.,0.,0.));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransform2 () 
{
  // Matriu de transformació de model
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.,0.,1.));
  transform = glm::rotate(transform, (float)M_PI, glm::vec3(0.,1.,0.));
  transform = glm::scale(transform, glm::vec3(scale1));
  transform = glm::translate(transform,-centreMod);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::terraTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform = glm::mat4(1.0f);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}


void MyGLWidget::resizeGL (int w, int h) 
{
  wGlobal=w;
  hGlobal=h;
  /*if(((double)w/(double)h)<1.0)
    FOVC=2.0 * ((180.0/M_PI)*(atan(tan(FOV/2.0)/(w/h))));
  else FOVC=FOV;*/
  glViewport(0, 0, w, h);
  projectTransform();
}

void MyGLWidget::doZoom (int cont) 
{

  if(cont<0) FOVC=FOVC*1.0001;
  else FOVC=FOVC*0.9999;
  projectTransform();
  
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  switch (event->key()) {
    case Qt::Key_Escape:
      exit(0);
    case Qt::Key_Z: { // escalar a més petit
      doZoom(1);
      updateGL();
      break;
    }
    case Qt::Key_X: { // escalar a més petit
      doZoom(-1);
      updateGL();
      break;
    }
    default: event->ignore(); break;
  }
}

void MyGLWidget::createBuffers () 
{
  //Dades Model
  
  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Model);
  glBindVertexArray(VAO_Model);
  
  glGenBuffers(1, &VBO_ModelPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_ModelCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
//Dades Model
  
  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Model2);
  glBindVertexArray(VAO_Model2);
  
  glGenBuffers(1, &VBO_ModelPos2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelPos2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_ModelCol2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_ModelCol2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Dades del terra
  // Dos VBOs, un amb posició i l'altre amb color
  glm::vec3 posterra[4] = {
	glm::vec3(-2.0, 0.0, -2.0),
	glm::vec3(-2.0, 0.0, 2.0),
	glm::vec3(2.0, 0.0, -2.0),
	glm::vec3(2.0, 0.0, 2.0)
  }; 
  glm::vec3 colterra[4] = {
	glm::vec3(1,0,1),
	glm::vec3(1,0,1),
	glm::vec3(1,0,1),
	glm::vec3(1,0,1)
  };

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders() 
{
  // Creem els shaders per al fragment shader i el vertex shader
  QGLShader fs (QGLShader::Fragment, this);
  QGLShader vs (QGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc = glGetUniformLocation(program->programId(), "proj");
  viewLoc = glGetUniformLocation(program->programId(), "view");
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;
    angleX += (e->y() - yClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  updateGL ();
}



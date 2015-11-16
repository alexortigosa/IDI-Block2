#define GLM_FORCE_RADIANS
#include <QGLWidget>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"

class MyGLWidget : public QGLWidget {
  Q_OBJECT

  public:
    MyGLWidget (QGLFormat &f, QWidget *parent=0);

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    //Función que define la proyección
    virtual void projectTransform();
    //Función que define la viw matrix
    virtual void viewTransform();
    //carrega model
    virtual void carregaModel();
    //calculo caja
    virtual void calculo_caja(Model *m, glm::vec3 *centreModAux, float *scale);

    virtual void mousePressEvent (QMouseEvent *e);

    virtual void mouseReleaseEvent( QMouseEvent *);

    virtual void mouseMoveEvent(QMouseEvent *e);
    

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();
    void modelTransformM ();
    void doZoom (int cont); 

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc;
    GLuint transLocM;
    GLuint projLoc;
    GLuint viewLoc;
    // VAO i VBO names
    GLuint VAO_Casa, VBO_CasaPos, VBO_CasaCol;
    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraCol;
    GLuint VAO_Model, VBO_ModelPos, VBO_ModelCol;
    int wGlobal, hGlobal;
    glm::vec3 centreMod; 
    float scale1;
    float h;
    // Program
    QGLShaderProgram *program;
    // Internal vars
    float scale,zNear,zFar;
    double RadioSphere;
    double distObs;
    float FOV, FOVC;
    glm::vec3 pos;
    //Modelo
    Model m;
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float angleY;
    float angleX;
};


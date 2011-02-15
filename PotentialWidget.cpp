/*#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>

PotentialWidget::PotentialWidget()
{
        QVBoxLayout *vl = new QVBoxLayout();
        scene = new PotentialScene(model,this);
        scene->setItemIndexMethod(QGraphicsScene::NoIndex);///?
        potentialViewMovable = new PotentialViewMovable(scene);
        vl->addWidget(potentialViewMovable);
//        connect(scene,SIGNAL(sceneRectChanged(const QRectF&)),potentialViewMovable,SLOT(resize()));
//        connect(scene,SIGNAL(sceneRectChanged(const QRectF&)),potentialViewMovable,SLOT(updateScene (const QList <QRectF>&)));
//        connect(scene,SIGNAL(sceneRectChanged(const QRectF&)),potentialViewMovable,SLOT(updateSceneRect (const QRectF&)));
//        connect(scene,SIGNAL(sceneRectChanged(const QRectF&)),potentialViewMovable,SLOT(invalidateScene(const QRectF&)));
        connect(scene,SIGNAL(sceneRectChanged(const QRectF&)),potentialViewMovable,SLOT(update(const QRectF&)));

        {
        QHBoxLayout *h = new QHBoxLayout();

        mouseAtX = new QLabel("x-coord");
        h->addWidget(mouseAtX);
        mouseAtY = new QLabel("y-coord");
        h->addWidget(mouseAtY);

        vl->addLayout(hl);
}

        connect(potentialViewMovable,SIGNAL(infoMouseMovedTo(QPointF)),
            this,SLOT(updateMouseMovedTo(QPointF)));

        QPushButton * reset = new QPushButton("&Reset Potential");	
        connect(reset,SIGNAL(clicked()),scene,SLOT(redrawU()));
        connect(reset,SIGNAL(clicked()),scene,SLOT(redrawEn()));
        vl->addWidget(reset);		

        QPushButton * addseg = new QPushButton("&Add Potential Segment");	
        connect(addseg,SIGNAL(clicked()),scene,SLOT(addNewSegment()));
        vl->addWidget(addseg);
}*/
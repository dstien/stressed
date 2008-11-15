// stressed - Stunts/4D [Sports] Driving resource editor
// Copyright (C) 2008 Daniel Stien <daniel@stien.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <QGLWidget>
#include <QPaintEvent>
#include <cmath>

#include "app/settings.h"
#include "materialsmodel.h"
#include "shapeview.h"
#include "verticesmodel.h"

// Convert 8-bit primitive index to unique RGB color used for picking.
#define CODE2COLOR(i) QColor(i & 0xE0, (i & 0x1C) << 3, (i & 0x3) << 6)
#define COLOR2CODE(c) (c[0] | c[1] >> 3 | c[2] >> 6)

const quint8 ShapeView::PATTERNS[6][0x80] = {
  { // Transparent
    0x00
  },
  { // Grate
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC,
    0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC
  },
  { // Grille
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F
  },
  { // Inverse grille
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0
  },
  { // Glass
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF,
    0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0xF3, 0x3C, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF, 0x3C, 0xCF
  },
  { // Inverse glass
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30,
    0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0x0C, 0xC3, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30, 0xC3, 0x30
  }
};

const float ShapeView::VERTEX_HIGHLIGHT_OFFSET = 20.0f;
const float ShapeView::PI2 = M_PI * 2.0f;
const float ShapeView::SPHERE_RADIUS_RATIO = 2.0f / 3.0f;

ShapeView::ShapeView(QWidget* parent)
: QAbstractItemView(parent)
{
  m_shapeModel = 0;
  m_currentPaintJob = 0;
  m_wireframe = false;
  m_showCullData = false;
  m_vertexSelection = 0;

  m_glWidget = new QGLWidget(this);
  m_glWidget->makeCurrent();
  m_glWidget->qglClearColor(Qt::white);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_FLAT);

  setViewport(m_glWidget);
}

void ShapeView::setModel(QAbstractItemModel* model)
{
  m_shapeModel = qobject_cast<ShapeModel*>(model);

  QAbstractItemView::setModel(model);
}

void ShapeView::reset()
{
  m_translation.reset();
  m_rotation.reset();

  if (m_shapeModel) {
    Vertex* bound = m_shapeModel->boundBox();
    m_translation.move(-((bound[4].y + bound[0].y) / 2), Matrix::AXIS_Y);
    m_translation.move(-distance(bound[2], bound[5]), Matrix::AXIS_Z);
    m_rotation.rotate(10.0f, Matrix::AXIS_X);
  }

  QAbstractItemView::reset();
}

void ShapeView::setCurrentPaintJob(int paintJob)
{
  m_currentPaintJob = qMax(0, paintJob - 1);
  viewport()->update();
}

void ShapeView::toggleWireframe(bool enable)
{
  m_wireframe = enable;
  glPolygonMode(GL_FRONT_AND_BACK, (enable ? GL_LINE : GL_FILL));
  viewport()->update();
}

void ShapeView::toggleShowCullData(bool enable)
{
  m_showCullData = enable;
  viewport()->update();
}

void ShapeView::updateGeometries()
{
  int width = viewport()->width();
  int height = qMax(1, viewport()->height());

  m_glWidget->makeCurrent();
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 5000.0f);

  glMatrixMode(GL_MODELVIEW);

  QAbstractItemView::updateGeometries();
}

void ShapeView::showEvent(QShowEvent* event)
{
  m_glWidget->makeCurrent();
  QAbstractItemView::showEvent(event);
}

void ShapeView::paintEvent(QPaintEvent* event)
{
  event->accept();

  draw(false);

  m_glWidget->swapBuffers();
}

void ShapeView::draw(bool pick)
{
  if (!m_shapeModel) {
    return;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glPushMatrix();
  m_translation.multMatrix();
  m_rotation.multMatrix();

  int i = 0;
  QItemSelectionModel* selections = selectionModel();

  foreach (const Primitive& primitive, *m_shapeModel->primitivesList()) {
    VerticesList* verticesList = primitive.verticesModel->verticesList();
    MaterialsList* materialsList = primitive.materialsModel->materialsList();

    int material = materialsList->at(m_currentPaintJob);
    QColor color;

    bool selected = false, pattern = false;

    if (pick) {
      m_glWidget->qglColor(CODE2COLOR(i));
    }
    else {
      if (m_vertexSelection && primitive.verticesModel == m_vertexSelection->model()) {
        foreach (const QModelIndex& index, m_vertexSelection->selectedRows()) {
          drawHighlightedVertex(verticesList->at(index.row()));
        }
      }

      if (selections->isSelected(m_shapeModel->index(i, 0))) {
        selected = true;

        if (m_showCullData) {
          drawCullData(primitive);
        }
      }
    }

    setMaterial(material, pattern, selected, pick);

    if (primitive.twoSided | m_wireframe) {
      glDisable(GL_CULL_FACE);
    }

    if (primitive.zBias) {
      glDepthRange(0.0f, 1.0f);
    }

    if (primitive.type == PRIM_TYPE_PARTICLE) {
      glBegin(GL_POINT);
      glVertex3s(verticesList->at(0).x, verticesList->at(0).y, -verticesList->at(0).z);
      glEnd();
    }
    else if (primitive.type == PRIM_TYPE_LINE) {
      glBegin(GL_LINES);
      foreach (const Vertex& vertex, *verticesList) {
        glVertex3s(vertex.x, vertex.y, -vertex.z);
      }
      glEnd();
    }
    else if (primitive.type > PRIM_TYPE_LINE && primitive.type < PRIM_TYPE_SPHERE) { // Polygon
      glBegin(GL_POLYGON);
      for (int j = verticesList->size() - 1; j >= 0; j--) {
        glVertex3s(verticesList->at(j).x, verticesList->at(j).y, -verticesList->at(j).z);
      }
      glEnd();
    }
    else if (m_wireframe && (primitive.type == PRIM_TYPE_SPHERE || primitive.type == PRIM_TYPE_WHEEL)) {
      glBegin(GL_LINE_STRIP);
      foreach (const Vertex& vertex, *verticesList) {
        glVertex3s(vertex.x, vertex.y, -vertex.z);
      }
      glEnd();
    }
    else if (primitive.type == PRIM_TYPE_SPHERE) {
      drawSphere(verticesList);
    }
    else if (primitive.type == PRIM_TYPE_WHEEL) {
      drawWheel(verticesList, material, pattern, selected, pick);
    }

    if (primitive.zBias) {
      glDepthRange(0.025f, 1.0f);
    }

    if (pattern) {
      glDisable(GL_POLYGON_STIPPLE);
    }

    if (primitive.twoSided | m_wireframe) {
      glEnable(GL_CULL_FACE);
    }

    i++;
  }

  glPopMatrix();
}

void ShapeView::drawSphere(const VerticesList* vertices)
{
  float radius = distance(vertices->at(0), vertices->at(1)) * SPHERE_RADIUS_RATIO;

  glPushMatrix();
  glTranslatef(vertices->at(0).x, vertices->at(0).y, -vertices->at(0).z);

  // Billboard face by using inverse shape rotation matrix.
  m_rotation.transpose().multMatrix();

  glBegin(GL_TRIANGLE_FAN);
  for (int j = 0; j < CIRCLE_STEPS; j++) {
    glVertex3f(
        cos((PI2 * (j + 1)) / CIRCLE_STEPS) * radius,
        sin((PI2 * (j + 1)) / CIRCLE_STEPS) * radius,
        0.0f);
  }
  glEnd();

  glPopMatrix();
}

void ShapeView::drawWheel(const VerticesList* vertices, int& material, bool& pattern, const bool& selected, const bool& pick)
{
  float radius2 = distance(vertices->at(0), vertices->at(1));
  float radius1 = radius2 * 0.6f;

  Vertex center = centroid(vertices->at(0), vertices->at(3));
  float halfWidth = distance(vertices->at(0), center);
  float x, y;

  glPushMatrix();
  glTranslatef(center.x, center.y, -center.z);

  // TODO: Check actual rotation.
  if (vertices->at(0).x - vertices->at(3).x) {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  }

  // Tyre tread
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius2, 0.0f, halfWidth);
  glVertex3f(radius2, 0.0f, -halfWidth);
  for (int i = 0; i < CIRCLE_STEPS; i++) {
    x = cos((PI2 * (i + 1)) / CIRCLE_STEPS) * radius2;
    y = sin((PI2 * (i + 1)) / CIRCLE_STEPS) * radius2;
    glVertex3f(x, y, halfWidth);
    glVertex3f(x, y, -halfWidth);
  }
  glEnd();

  if (material < MaterialsModel::VAL_MAX) {
    setMaterial(++material, pattern, selected, pick);
  }

  // Inner tyre
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius1, 0.0f, -halfWidth);
  glVertex3f(radius2, 0.0f, -halfWidth);
  for (int i = 0; i < CIRCLE_STEPS; i++) {
    x = cos((PI2 * (i + 1)) / CIRCLE_STEPS);
    y = -sin((PI2 * (i + 1)) / CIRCLE_STEPS);
    glVertex3f(x * radius1, y * radius1, -halfWidth);
    glVertex3f(x * radius2, y * radius2, -halfWidth);
  }
  glEnd();
  // Outer tyre
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius1, 0.0f, halfWidth);
  glVertex3f(radius2, 0.0f, halfWidth);
  for (int i = 0; i < CIRCLE_STEPS; i++) {
    x = cos((PI2 * (i + 1)) / CIRCLE_STEPS);
    y = sin((PI2 * (i + 1)) / CIRCLE_STEPS);
    glVertex3f(x * radius1, y * radius1, halfWidth);
    glVertex3f(x * radius2, y * radius2, halfWidth);
  }
  glEnd();

  if (material < MaterialsModel::VAL_MAX) {
    setMaterial(++material, pattern, selected, pick);
  }

  // Inner rim
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i < CIRCLE_STEPS; i++) {
    glVertex3f(
        sin((PI2 * (i + 1)) / CIRCLE_STEPS) * radius1,
        cos((PI2 * (i + 1)) / CIRCLE_STEPS) * radius1,
        -halfWidth);
  }
  glEnd();
  // Outer rim
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i < CIRCLE_STEPS; i++) {
    glVertex3f(
        cos((PI2 * (i + 1)) / CIRCLE_STEPS) * radius1,
        sin((PI2 * (i + 1)) / CIRCLE_STEPS) * radius1,
        halfWidth);
  }
  glEnd();

  glPopMatrix();
}

void ShapeView::drawHighlightedVertex(const Vertex& vertex)
{
  glDepthRange(0.0f, 1.0f);

  glBegin(GL_LINES);
  m_glWidget->qglColor(Qt::red);
  glVertex3f(vertex.x, vertex.y + VERTEX_HIGHLIGHT_OFFSET, -vertex.z);
  glVertex3f(vertex.x, vertex.y - VERTEX_HIGHLIGHT_OFFSET, -vertex.z);

  m_glWidget->qglColor(Qt::blue);
  glVertex3f(vertex.x + VERTEX_HIGHLIGHT_OFFSET, vertex.y, -vertex.z);
  glVertex3f(vertex.x - VERTEX_HIGHLIGHT_OFFSET, vertex.y, -vertex.z);

  m_glWidget->qglColor(Qt::green);
  glVertex3f(vertex.x, vertex.y, -vertex.z + VERTEX_HIGHLIGHT_OFFSET);
  glVertex3f(vertex.x, vertex.y, -vertex.z - VERTEX_HIGHLIGHT_OFFSET);
  glEnd();

  glDepthRange(0.025f, 1.0f);
}

void ShapeView::drawCullData(const Primitive& primitive)
{
  float radius1 = 40.0f;
  float radius2 = 60.0f;
  float radius3 = 80.0f;
  float x, z;
  static const int steps = 15;
  Vertex center = centroid(primitive);

  if (m_wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // 1+
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius2 + center.x, center.y, -center.z);
  glVertex3f(radius3 + center.x, center.y, -center.z);
  for (int j = 0; j < steps; j++) {
    if (primitive.cull1 & (1 << (steps + steps - j + 1))) {
      if (j % 2) m_glWidget->qglColor(Qt::darkRed);
      else       m_glWidget->qglColor(Qt::red);
    }
    else {
      if (j % 2) m_glWidget->qglColor(Qt::darkGreen);
      else       m_glWidget->qglColor(Qt::green);
    }
    x = cos((PI2 * (j + 1)) / steps);
    z = -sin((PI2 * (j + 1)) / steps);
    glVertex3f(x * radius2 + center.x, center.y, z * radius2 + -center.z);
    glVertex3f(x * radius3 + center.x, center.y, z * radius3 + -center.z);
  }
  glEnd();
  // 1-
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius2 + center.x, center.y, -center.z);
  glVertex3f(radius3 + center.x, center.y, -center.z);
  for (int j = 0; j < steps; j++) {
    if (primitive.cull1 & (1 << (j + 2))) {
      if (j % 2) m_glWidget->qglColor(Qt::darkRed);
      else       m_glWidget->qglColor(Qt::red);
    }
    else {
      if (j % 2) m_glWidget->qglColor(Qt::darkGreen);
      else       m_glWidget->qglColor(Qt::green);
    }
    x = cos((PI2 * (j + 1)) / steps);
    z = sin((PI2 * (j + 1)) / steps);
    glVertex3f(x * radius2 + center.x, center.y, z * radius2 + -center.z);
    glVertex3f(x * radius3 + center.x, center.y, z * radius3 + -center.z);
  }
  glEnd();
  // 2+
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius1 + center.x, center.y, -center.z);
  glVertex3f(radius2 + center.x, center.y, -center.z);
  for (int j = 0; j < steps; j++) {
    if (primitive.cull2 & (1 << (steps + steps - j + 1))) {
      if (j % 2) m_glWidget->qglColor(Qt::darkMagenta);
      else       m_glWidget->qglColor(Qt::magenta);
    }
    else {
      if (j % 2) m_glWidget->qglColor(Qt::darkYellow);
      else       m_glWidget->qglColor(Qt::yellow);
    }
    x = cos((PI2 * (j + 1)) / steps);
    z = -sin((PI2 * (j + 1)) / steps);
    glVertex3f(x * radius1 + center.x, center.y, z * radius1 + -center.z);
    glVertex3f(x * radius2 + center.x, center.y, z * radius2 + -center.z);
  }
  glEnd();
  // 2-
  glBegin(GL_QUAD_STRIP);
  glVertex3f(radius1 + center.x, center.y, -center.z);
  glVertex3f(radius2 + center.x, center.y, -center.z);
  for (int j = 0; j < steps; j++) {
    if (primitive.cull2 & (1 << (j + 2))) {
      if (j % 2) m_glWidget->qglColor(Qt::darkMagenta);
      else       m_glWidget->qglColor(Qt::magenta);
    }
    else {
      if (j % 2) m_glWidget->qglColor(Qt::darkYellow);
      else       m_glWidget->qglColor(Qt::yellow);
    }
    x = cos((PI2 * (j + 1)) / steps);
    z = sin((PI2 * (j + 1)) / steps);
    glVertex3f(x * radius1 + center.x, center.y, z * radius1 + -center.z);
    glVertex3f(x * radius2 + center.x, center.y, z * radius2 + -center.z);
  }
  glEnd();

  if (m_wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
}

void ShapeView::setMaterial(const int& material, bool& pattern, const bool& selected, const bool& pick)
{
  if (!pick) {
    QColor color = Settings::PALETTE[Settings::MATERIALS[material].color];

    if (selected) {
      color.setRed(qMin(0xFF, color.red() + 0x7F));
      color.setGreen(qMax(0, color.green() - 0x7F));
      color.setBlue(qMax(0, color.blue() - 0x7F));
    }

    m_glWidget->qglColor(color);
  }

  if (!m_wireframe) {
    if (Settings::MATERIALS[material].pattern && (Settings::MATERIALS[material].pattern <= 6)) {
      if (!pattern) {
        glEnable(GL_POLYGON_STIPPLE);
      }
      pattern = true;
      glPolygonStipple(PATTERNS[Settings::MATERIALS[material].pattern - 1]);
    }
    else if (pattern) {
      glDisable(GL_POLYGON_STIPPLE);
      pattern = false;
    }
  }
}

int ShapeView::pick()
{
  draw(true);

  GLubyte pixel[3];
  glReadPixels(m_lastMousePosition.x(), viewport()->height() - m_lastMousePosition.y(),
      1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixel);

  return COLOR2CODE(pixel);
}

Vertex ShapeView::centroid(const Primitive& primitive)
{
  Vertex res; // TODO: Use float vertex.
  res.x = res.y = res.z = 0;

  if (primitive.type == PRIM_TYPE_PARTICLE || primitive.type == PRIM_TYPE_SPHERE) {
    return primitive.verticesModel->verticesList()->at(0);
  }
  else if (primitive.type == PRIM_TYPE_LINE) {
    return centroid(primitive.verticesModel->verticesList()->at(0), primitive.verticesModel->verticesList()->at(1));
  }
  else if (primitive.type > PRIM_TYPE_LINE && primitive.type < PRIM_TYPE_SPHERE) { // Polygon
    foreach (Vertex vertex, *primitive.verticesModel->verticesList()) {
      res.x += vertex.x;
      res.y += vertex.y;
      res.z += vertex.z;
    }
    res.x /= primitive.verticesModel->verticesList()->size();
    res.y /= primitive.verticesModel->verticesList()->size();
    res.z /= primitive.verticesModel->verticesList()->size();
  }
  else if (primitive.type == PRIM_TYPE_WHEEL) {
    return centroid(primitive.verticesModel->verticesList()->at(0), primitive.verticesModel->verticesList()->at(3));
  }

  return res;
}

Vertex ShapeView::centroid(const Vertex& v1, const Vertex& v2)
{
  Vertex res; // TODO: Use float vertex.
  res.x = res.y = res.z = 0;

  res.x = (v1.x + v2.x) / 2;
  res.y = (v1.y + v2.y) / 2;
  res.z = (v1.z + v2.z) / 2;

  return res;
}

float ShapeView::distance(const Vertex& v1, const Vertex& v2)
{
  float dx = v2.x - v1.x;
  float dy = v2.y - v1.y;
  float dz = v2.z - v1.z;

  return sqrt(dx * dx + dy * dy + dz * dz);
} 

void ShapeView::mousePressEvent(QMouseEvent* event)
{
  event->accept();
  m_lastMousePosition = event->pos();

  int row = pick();

  if (m_shapeModel && row < ShapeModel::ROWS_MAX) {
    selectionModel()->setCurrentIndex(m_shapeModel->index(row, 0),
        QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
  }
}

void ShapeView::mouseMoveEvent(QMouseEvent* event)
{
  event->accept();
  QPoint delta = event->pos() - m_lastMousePosition;
  m_lastMousePosition = event->pos();

  if ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)) {
    m_translation.move(delta.y() * 5.0f, Matrix::AXIS_Y);
  }
  else if (event->buttons() & Qt::LeftButton) {
    m_rotation.rotate(-delta.x() * 0.25f, Matrix::AXIS_Y);
    m_rotation.rotate(-delta.y() * 0.25f, Matrix::AXIS_X);
  }
  else if (event->buttons() & Qt::RightButton) {
    m_rotation.rotate(delta.x() * 0.25f, Matrix::AXIS_Z);
  }
  else if (event->buttons() & Qt::MidButton) {
    m_translation.move(delta.x() * 5.0f, Matrix::AXIS_X);
    m_translation.move(-delta.y() * 5.0f, Matrix::AXIS_Z);
  }

  viewport()->update();
}

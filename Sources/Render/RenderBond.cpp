#include "Render/RenderAtom.h"
#include "Render/RenderBond.h"
#include "Render/RenderCylinder.h"

const GLfloat Render::Bond::DEFAULT_THIKNESS = 0.15f;

Render::Bond::Bond(Chemistry::Molecule* molecule, int index1, int index2):
    isSelected_(false)
{
  molecule_ = molecule;
  beginAtomId_ = index1;
  endAtomId_ = index2;
}

void Render::Bond::draw(Quality quality) const
{
  Render::Atom atom1(molecule_->atom(beginAtomId_));
  Render::Atom atom2(molecule_->atom(endAtomId_));
  Render::Material material1(atom1.color(), true);
  Render::Material material2(atom2.color(), true);

  // Compute the centre of bond
  Eigen::Vector3f vec1 = atom2.centre() - atom1.centre();
  vec1 = vec1 * (vec1.norm() - atom2.drawRadius()) / vec1.norm();
  vec1 = vec1 + atom1.centre();
  Eigen::Vector3f vec2 = atom1.centre() - atom2.centre();
  vec2 = vec2 * (vec2.norm() - atom1.drawRadius()) / vec2.norm();
  vec2 = vec2 + atom2.centre();
  Eigen::Vector3f vMiddle = (vec1 + vec2) / 2;

  Render::Cylinder cylinder1(atom1.centre(), vMiddle, DEFAULT_THIKNESS, material1);
  Render::Cylinder cylinder2(vMiddle, atom2.centre(), DEFAULT_THIKNESS, material2);
  cylinder1.draw(STYLE_FILL, quality);
  cylinder2.draw(STYLE_FILL, quality);
}

void Render::Bond::drawSelection(Render::Quality quality) const
{
  /*  Stereometry::Point* point1;
  Stereometry::Point* point2;
  // Enable blending
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
  Material material(Color::selection(),
                    Material::defaultComponent(Material::DIFFUSE),
                    Material::defaultComponent(Material::EMISSION),
                    Material::defaultComponent(Material::SPECULAR),
                    Material::defaultShininess());
  Stereometry::Vector v(atom1_->centre(), atom2_->centre());
  if (atom1_->isSelected())
    point1 = new Stereometry::Point(v.pointAt(atom1_->radius() + 0.05f));
  else
    point1 = new Stereometry::Point(v.origin());
  if (atom2_->isSelected())
    point2 = new Stereometry::Point(v.pointAt(v.modulus() - (atom2_->radius() + 0.05f)));
  else
    point2 = new Stereometry::Point(v.terminus());
  Cylinder cylinderRender(*point1, *point2, DEFAULT_THIKNESS + 0.1f, material);
  cylinderRender.draw(STYLE_FILL, quality);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);*/
}

bool Render::Bond::isSelected() const
{
  return isSelected_;
}

void Render::Bond::setSelected(bool selected)
{
  if (selected == true)
    isSelected_ = true;
  else
    isSelected_ = false;
}

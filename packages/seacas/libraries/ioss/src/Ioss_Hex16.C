// Copyright(C) 1999-2017 National Technology & Engineering Solutions
// of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
// NTESS, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of NTESS nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Ioss_CodeTypes.h"           // for IntVector
#include "Ioss_ElementTopology.h"     // for ElementTopology
#include <Ioss_ElementVariableType.h> // for ElementVariableType
#include <Ioss_Hex16.h>
#include <cassert> // for assert

//------------------------------------------------------------------------
// Define a variable type for storage of this elements connectivity
namespace Ioss {
  const char *Hex16::name = "hex16";
  class St_Hex16 : public ElementVariableType
  {
  public:
    static void factory() { static St_Hex16 registerThis; }

  protected:
    St_Hex16() : ElementVariableType(Ioss::Hex16::name, 16) {}
  };
} // namespace Ioss

// ========================================================================
namespace {
  struct Constants
  {
    static const int nnode     = 16;
    static const int nedge     = 12;
    static const int nedgenode = 3;
    static const int nface     = 6;
    static const int nfacenode = 8;
    static const int nfaceedge = 4;
    static int       edge_node_order[nedge][nedgenode];
    static int       face_node_order[nface][nfacenode];
    static int       face_edge_order[nface][nfaceedge];
    static int       nodes_per_face[nface + 1];
    static int       edges_per_face[nface + 1];
  };

  // Edge numbers are zero-based [0..number_edges]
  int Constants::edge_node_order[nedge][nedgenode] = // [edge][edge_node]
      {{0, 1, 8},  {1, 2, 9},  {2, 3, 10}, {3, 0, 11}, {4, 5, 12}, {5, 6, 13},
       {6, 7, 14}, {7, 4, 15}, {0, 4, -1}, {1, 5, -1}, {2, 6, -1}, {3, 7, -1}};

  // Face numbers are zero-based [0..number_faces]
  int Constants::face_node_order[nface][nfacenode] = // [face][face_node]
      {{0, 1, 5, 4, 8, 12, -1, -1}, {1, 2, 6, 5, 9, 13, -1, -1}, {2, 3, 7, 6, 10, 14, -1, -1},
       {3, 0, 4, 7, 3, 11, 15, -1}, {0, 3, 2, 1, 11, 10, 9, 8},  {4, 5, 6, 7, 12, 13, 14, 15}};

  int Constants::face_edge_order[nface][nfaceedge] = // [face][face_edge]
      {{0, 9, 4, 8}, {1, 10, 5, 9}, {2, 11, 6, 10}, {3, 8, 7, 11}, {3, 2, 1, 0}, {4, 5, 6, 7}};

  // face 0 returns number of nodes for all faces if homogenous
  //        returns -1 if faces have differing topology
  int Constants::nodes_per_face[nface + 1] = {-1, 6, 6, 6, 6, 8, 8};

  // face 0 returns number of edges for all faces if homogenous
  //        returns -1 if faces have differing topology
  int Constants::edges_per_face[nface + 1] = {4, 4, 4, 4, 4, 4, 4};
} // namespace

void Ioss::Hex16::factory()
{
  static Ioss::Hex16 registerThis;
  Ioss::St_Hex16::factory();
}

Ioss::Hex16::Hex16() : Ioss::ElementTopology(Ioss::Hex16::name, "Hexahedron_16")
{
  Ioss::ElementTopology::alias(Ioss::Hex16::name, "Solid_Hex_16_3D");
}

Ioss::Hex16::~Hex16() = default;

int Ioss::Hex16::parametric_dimension() const { return 3; }
int Ioss::Hex16::spatial_dimension() const { return 3; }
int Ioss::Hex16::order() const { return 1; }

int Ioss::Hex16::number_corner_nodes() const { return 8; }
int Ioss::Hex16::number_nodes() const { return Constants::nnode; }
int Ioss::Hex16::number_edges() const { return Constants::nedge; }
int Ioss::Hex16::number_faces() const { return Constants::nface; }

int Ioss::Hex16::number_nodes_edge(int edge) const
{
  // edge is 1-based.  0 passed in for all edges.
  assert(edge >= 0 && edge <= number_edges());
  if (edge == 0) {
    return -1;
  }
  else if (edge <= 8) {
    return 3;
  }
  else {
    return 2;
  }
}

int Ioss::Hex16::number_nodes_face(int face) const
{
  // face is 1-based.  0 passed in for all faces.
  assert(face >= 0 && face <= number_faces());
  return Constants::nodes_per_face[face];
}

int Ioss::Hex16::number_edges_face(int face) const
{
  // face is 1-based.  0 passed in for all faces.
  assert(face >= 0 && face <= number_faces());
  return Constants::edges_per_face[face];
}

Ioss::IntVector Ioss::Hex16::edge_connectivity(int edge_number) const
{
  assert(edge_number > 0 && edge_number <= Constants::nedge);
  Ioss::IntVector connectivity(number_nodes_edge(edge_number));

  for (int i = 0; i < number_nodes_edge(edge_number); i++) {
    connectivity[i] = Constants::edge_node_order[edge_number - 1][i];
  }

  return connectivity;
}

Ioss::IntVector Ioss::Hex16::face_connectivity(int face_number) const
{
  assert(face_number > 0 && face_number <= number_faces());
  Ioss::IntVector connectivity(Constants::nodes_per_face[face_number]);

  for (int i = 0; i < Constants::nodes_per_face[face_number]; i++) {
    connectivity[i] = Constants::face_node_order[face_number - 1][i];
  }

  return connectivity;
}

Ioss::IntVector Ioss::Hex16::element_connectivity() const
{
  Ioss::IntVector connectivity(number_nodes());
  for (int i = 0; i < number_nodes(); i++) {
    connectivity[i] = i;
  }
  return connectivity;
}

Ioss::ElementTopology *Ioss::Hex16::face_type(int face_number) const
{
  // face_number == 0 returns topology for all faces if
  // all faces are the same topology; otherwise, returns nullptr
  // face_number is 1-based.
  assert(face_number >= 0 && face_number <= number_faces());

  if (face_number == 0) {
    return (Ioss::ElementTopology *)nullptr;
  }
  if (face_number <= 4) {
    return Ioss::ElementTopology::factory("quad6");
  }
  return Ioss::ElementTopology::factory("quad8");
}

Ioss::ElementTopology *Ioss::Hex16::edge_type(int edge_number) const
{
  // edge_number == 0 returns topology for all edges if
  // all edges are the same topology; otherwise, returns nullptr
  // edge_number is 1-based.
  assert(edge_number >= 0 && edge_number <= number_edges());

  if (edge_number == 0) {
    return (Ioss::ElementTopology *)nullptr;
  }
  if (edge_number <= 8) {
    return Ioss::ElementTopology::factory("edge3");
  }
  return Ioss::ElementTopology::factory("edge2");
}

Ioss::IntVector Ioss::Hex16::face_edge_connectivity(int face_number) const
{
  assert(face_number > 0 && face_number <= Constants::nface);

  int             nface_edge = number_edges_face(face_number);
  Ioss::IntVector fcon(nface_edge);

  for (int i = 0; i < nface_edge; i++) {
    fcon[i] = Constants::face_edge_order[face_number - 1][i];
  }

  return fcon;
}

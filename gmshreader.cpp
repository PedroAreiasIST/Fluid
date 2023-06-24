#include "GraphDatabase/include/Utils/Utils.hpp"
#include <array>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <chrono>
using namespace std;
using namespace Utils;
/*
 * GMSH data
 */
struct gmsh
{
  // types
  using INT = size_t;
  using coordt = std::array<double, 3>;
  // data
  unordered_map<INT, string> groupofnumber;
  vector<coordt> x;
  vector<std::pair<string, INT>> gnodes;
  vector<array<INT, 1>> vertices;
  vector<INT> gvertices;
  vector<array<INT, 2>> lines;
  vector<INT> glines;
  vector<array<INT, 3>> triangles;
  vector<INT> gtriangles;
  vector<array<INT, 4>> quads;
  vector<INT> gquads;
  vector<array<INT, 4>> tets;
  vector<INT> gtets;
  vector<array<INT, 8>> hexahedra;
  vector<INT> ghexahedra;
  vector<array<INT, 6>> wedges;
  vector<INT> gwedges;
};

ostream &
operator<<(ostream &os, const gmsh &rhs)
{
  os << rhs.groupofnumber << endl;
  os << rhs.x << endl;
  os << rhs.vertices << endl;
  os << rhs.lines << endl;
  os << rhs.triangles << endl;
  os << rhs.quads << endl;
  os << rhs.tets << endl;
  os << rhs.hexahedra << endl;
  os << rhs.wedges << endl;
  os << rhs.gvertices << endl;
  os << rhs.glines << endl;
  os << rhs.gtriangles << endl;
  os << rhs.gquads << endl;
  os << rhs.gtets << endl;
  os << rhs.ghexahedra << endl;
  os << rhs.gwedges << endl;
  return os;
}

istream &
operator>>(istream &is, gmsh &rhs)
{
  is >> rhs.groupofnumber;
  is >> rhs.x;
  is >> rhs.vertices;
  is >> rhs.lines;
  is >> rhs.triangles;
  is >> rhs.quads;
  is >> rhs.tets;
  is >> rhs.hexahedra;
  is >> rhs.wedges;
  return is;
}

size_t
trianglereadfromoriginal(const string &basname, gmsh &vm)
{
  cout << "Basname=" << basname << endl;
  cout << "nodefile=" << basname + ".node" << endl;
  ifstream nodefile{trim(basname + ".node")};
  ifstream elemefile{trim(basname + ".ele")};
  cout << "nodefile=" << basname + ".node" << endl;
  string line{};
  size_t nno;
  nodefile >> nno;
  cout << "nno=" << nno << endl;
  vm.x.resize(nno);
  vm.gvertices.resize(nno);
  std::getline(nodefile, line);
  cout << "nno=" << nno << endl;
  for (size_t i = 0; i < nno; ++i)
  {
    size_t nn;
    nodefile >> nn >> vm.x[i];
    cout << "nn=" << nn << " vm.x=" << vm.x[i] << endl;
  }
  // min, max coord
  array<double, 2> xmin;
  array<double, 2> xmax;
  xmin[0] = vm.x[0][0];
  xmin[1] = vm.x[0][1];
  xmax[0] = vm.x[0][0];
  xmax[1] = vm.x[0][1];
  for (size_t i = 1; i != nno; ++i)
  {
    xmin[0] = std::min(xmin[0], vm.x[i][0]);
    xmin[1] = std::min(xmin[1], vm.x[i][1]);
    xmax[0] = std::max(xmax[0], vm.x[i][0]);
    xmax[1] = std::max(xmax[1], vm.x[i][1]);
  }
  for (size_t i = 0; i != nno; ++i)
  {
    if (vm.x[i][0] == xmin[0])
      vm.gnodes.push_back(std::make_pair("left", i));
    if (vm.x[i][0] == xmax[0])
      vm.gnodes.push_back(std::make_pair("right", i));
    if (vm.x[i][1] == xmin[1])
      vm.gnodes.push_back(std::make_pair("bottom", i));
    if (vm.x[i][1] == xmax[1])
      vm.gnodes.push_back(std::make_pair("top", i));
    if (abs(vm.x[i][2] - 2) < 1.0e-9)
      vm.gnodes.push_back(std::make_pair("airfoil", i));
    if((vm.x[i][0]==xmin[0])&&(vm.x[i][1]==xmin[1]))
      vm.gnodes.push_back(std::make_pair("bottom_left",i));
    if((vm.x[i][0]==xmax[0])&&(vm.x[i][1]==xmin[1]))
      vm.gnodes.push_back(std::make_pair("bottom_right",i));
    if((vm.x[i][0]==xmax[0])&&(vm.x[i][1]==xmax[1]))
      vm.gnodes.push_back(std::make_pair("top_right",i));
    if((vm.x[i][0]==xmin[0])&&(vm.x[i][1]==xmax[1]))
      vm.gnodes.push_back(std::make_pair("top_left",i));
    vm.x[i][2] = 0.0;
  }
  nodefile.close();
  size_t ntri;
  elemefile >> ntri;
  vm.triangles.resize(ntri);
  std::getline(elemefile, line);
  cout << "ntri" << ntri << endl;
  size_t tag{0};
  vm.groupofnumber[0] = "elements";
  for (auto i = 0; i != ntri; ++i)
  {
    size_t nt;
    elemefile >> nt >> vm.triangles[i];
    vm.gtriangles.push_back(0);
  }
  return 0;
}

size_t
gmshreadfromoriginal(ifstream &input_file, gmsh &vm)
{
  if (!input_file.is_open())
  {
    return 1;
  }
  // Format of the file
  string line;
  getline(input_file, line);
  double fmt;
  input_file >> fmt;
  if ((size_t)fmt != 2)
    return 2;
  getline(input_file, line);
  getline(input_file, line);
  getline(input_file, line);
  // Groups
  size_t ng{0};
  input_file >> ng;
  for (auto i = 0; i != ng; ++i)
  {
    size_t bb = 0;
    size_t tag = 0;
    string nametag;
    input_file >> bb >> tag >> nametag;
    stringremovechars(nametag, "\"");
    vm.groupofnumber[tag] = nametag;
  }
  // Coordinates
  std::getline(input_file, line);
  std::getline(input_file, line);
  size_t nno;
  input_file >> line >> nno;
  vm.x.resize(nno);
  for (size_t i = 0; i < nno; ++i)
  {
    size_t nn;
    input_file >> nn >> vm.x[i];
    if (nn - 1 != i)
      return 3;
  }
  // Generalized elements
  std::getline(input_file, line);
  std::getline(input_file, line);
  size_t ncells;
  input_file >> line >> ncells;
  vm.vertices.reserve(ncells);
  vm.lines.reserve(ncells);
  vm.triangles.reserve(ncells);
  vm.quads.reserve(ncells);
  vm.tets.reserve(ncells);
  for (size_t i = 0; i < ncells; ++i)
  {
    size_t num;
    input_file >> num;
    if (num - 1 != i)
      return 4;
    size_t typ;
    input_file >> typ;
    size_t ntags;
    input_file >> ntags;
    if (ntags != 2)
      return 5;
    size_t tag1, tag2;
    input_file >> tag1;
    if (tag1 < 0)
      return 6;
    input_file >> tag2;
    switch (typ)
    {
    case 1:
      array<size_t, 2> t2;
      input_file >> t2;
      vm.lines.push_back(t2);
      vm.glines.push_back(tag1);
      break;
    case 2:
      array<size_t, 3> t3;
      input_file >> t3;
      vm.triangles.push_back(t3);
      vm.gtriangles.push_back(tag1);
      break;
    case 3:
      array<size_t, 4> t4;
      input_file >> t4;
      vm.quads.push_back(t4);
      vm.gquads.push_back(tag1);
      break;
    case 4:
      array<size_t, 4> t4b;
      input_file >> t4b;
      vm.tets.push_back(t4b);
      vm.gtets.push_back(tag1);
      break;
    case 5:
      array<size_t, 8> t8;
      input_file >> t8;
      vm.hexahedra.push_back(t8);
      vm.ghexahedra.push_back(tag1);
      break;
    case 6:
      array<size_t, 6> t6;
      input_file >> t6;
      vm.wedges.push_back(t6);
      vm.gwedges.push_back(tag1);
      break;
    case 15:
      array<size_t, 1> t1;
      input_file >> t1;
      vm.vertices.push_back(t1);
      vm.gvertices.push_back(tag1);
      break;
    default:
      return 7;
      break;
    }
  }
  return 0;
}

/// @brief
/// @param filename
/// @param out

void gmshsendtosimplas(const string &filename, gmsh &out)
{
  ofstream file(filename + ".malha");
  file << "THIS FILE CONTAINS:\n";
  file << "1)COORDINATES\n";
  file << "2)PSEUDO - ELEMENT CONNECTIVITIES\n";
  file << "3)NODAL GROUPS\n";
  file << "4)ELEMENT GROUPS\n";
  file << "\n";
  file << "DO NOT TOUCH IT, SINCE IT IS FIXED FORMAT TO SPEED\n";
  file << "UP I / O OPERATIONS\n";
  file << "\n";
  file << "WITH FORMAT E15.7 AND I7\n";
  file << "\n";
  file << "\n";
  file << "COORDINATES (NODE NUMBER, X1,X2,X3)\n";
  file << std::setw(8) << out.x.size()
       << " -> THIS IS THE TOTAL NUMBER OF NODES\n";
  for (size_t i = 0; i < out.x.size(); ++i)
  {
    file << std::setw(8) << i + 1 << "   " << std::scientific
         << std::setprecision(7) << out.x[i][0] << "   " << out.x[i][1] << "   "
         << out.x[i][2] << endl;
  }
  file << " ELEMENTS (ELEMENT NUMBER, TOPOLOGY TYPE, CONNECTIVITIES, MAYBE "
          "NORMAL VECTOR)"
       << endl;
  size_t nel = out.vertices.size() + out.lines.size() + out.triangles.size() +
               out.quads.size() + out.tets.size() + out.wedges.size() +
               out.hexahedra.size();
  file << std::setw(8) << nel << " -> THIS IS THE TOTAL NUMBER OF ELEMENTS\n";
  size_t iel{0};
  for (size_t i = 0; i < out.vertices.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  1  " << std::setw(8) << out.vertices[i]
         << out.groupofnumber[out.gvertices[i]] << endl;
  }
  for (size_t i = 0; i < out.lines.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  2  " << std::setw(8) << out.lines[i]
         << out.groupofnumber[out.glines[i]] << endl;
  }
  for (size_t i = 0; i < out.triangles.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  3  " << std::setw(8) << out.triangles[i]
         << out.groupofnumber[out.gtriangles[i]] << endl;
  }
  for (size_t i = 0; i < out.quads.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  4  " << std::setw(8) << out.quads[i]
         << out.groupofnumber[out.gquads[i]] << endl;
  }
  for (size_t i = 0; i < out.tets.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  5  " << std::setw(8) << out.tets[i]
         << out.groupofnumber[out.gtets[i]] << endl;
  }
  for (size_t i = 0; i < out.hexahedra.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  6  " << std::setw(8) << out.hexahedra[i]
         << out.groupofnumber[out.ghexahedra[i]] << endl;
  }
  for (size_t i = 0; i < out.wedges.size(); ++i)
  {
    file << std::setw(8) << ++iel << "  7  " << std::setw(8) << out.wedges[i]
         << out.groupofnumber[out.gwedges[i]] << endl;
  }
  file << "NODAL GROUPS" << endl;
  size_t ng = out.vertices.size() * 1 + out.lines.size() * 2 +
              out.triangles.size() * 3 + out.quads.size() * 4 +
              out.tets.size() * 4 + out.wedges.size() * 6 +
              out.hexahedra.size() * 8 + out.gnodes.size();
  file << std::setw(8) << ng
       << " -> THIS IS THE TOTAL NUMBER OF NODES IN GROUPS\n";
  for (size_t i = 0; i < out.gnodes.size(); ++i)
  {
    file << std::setw(8) << out.gnodes[i].second + 1 << " " << trim(out.gnodes[i].first) << endl;
  }
  for (size_t i = 0; i < out.vertices.size(); ++i)
  {
    for (size_t j = 0; j < out.vertices[i].size(); ++j)
      file << std::setw(8) << out.vertices[i][j] << "   "
           << out.groupofnumber[out.gvertices[i]] << endl;
  }
  for (size_t i = 0; i < out.lines.size(); ++i)
  {
    for (size_t j = 0; j < out.lines[i].size(); ++j)
      file << std::setw(8) << out.lines[i][j] << "   "
           << out.groupofnumber[out.glines[i]] << endl;
  }
  for (size_t i = 0; i < out.triangles.size(); ++i)
  {
    for (size_t j = 0; j < out.triangles[i].size(); ++j)
      file << std::setw(8) << out.triangles[i][j] << "   "
           << out.groupofnumber[out.gtriangles[i]] << endl;
  }
  for (size_t i = 0; i < out.quads.size(); ++i)
  {
    for (size_t j = 0; j < out.quads[i].size(); ++j)
      file << std::setw(8) << out.quads[i][j] << "   "
           << out.groupofnumber[out.gquads[i]] << endl;
  }
  for (size_t i = 0; i < out.tets.size(); ++i)
  {
    for (size_t j = 0; j < out.tets[i].size(); ++j)
      file << std::setw(8) << out.tets[i][j] << "   "
           << out.groupofnumber[out.gtets[i]] << endl;
  }
  for (size_t i = 0; i < out.hexahedra.size(); ++i)
  {
    for (size_t j = 0; j < out.hexahedra[i].size(); ++j)
      file << std::setw(8) << out.hexahedra[i][j] << "   "
           << out.groupofnumber[out.ghexahedra[i]] << endl;
  }
  for (size_t i = 0; i < out.wedges.size(); ++i)
  {
    for (size_t j = 0; j < out.wedges[i].size(); ++j)
      file << std::setw(8) << out.wedges[i][j] << "   "
           << out.groupofnumber[out.gwedges[i]] << endl;
  }
}

inline size_t access(size_t nrows, size_t row, size_t col)
{
  return row + col * nrows;
}

int main()
{
  gmsh vm;
  bool iftriangle = true;
  if (iftriangle)
  {
    string file1{getlistoffileswithextension("1.poly")[0]};
    string file2{getlistoffileswithextension("1.ele")[0]};
    string basname;
    cleanextension(file1, ".poly");
    cleanextension(file2, ".ele");
    basname = trim(file1);
    cout << "Found the file: " << basname << endl;
    trianglereadfromoriginal(basname, vm);
    gmshsendtosimplas(basname, vm);
  }
  else
  {
    size_t e;
    string line, ifile, ofile;
    string filename{getlistoffileswithextension("msh")[0]};
    cleanextension(filename, "msh");
    cleanextension(filename, ".");
    ifile = filename + ".msh";
    cout << "Found the file: " << ifile << endl;
    ifstream input_file{ifile};
    if (!input_file.is_open())
    {
      ifile = "assembly1.msh";
      input_file.open(ifile);
    }
    e = gmshreadfromoriginal(input_file, vm);
    cout << "Error when reading gmsh file=" << e << endl;
    gmshsendtosimplas(filename, vm);
  }
}

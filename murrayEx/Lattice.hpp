#pragma once

#include <complex>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <cmath>
#include <unsupported/Eigen/MatrixFunctions>

constexpr std::complex<double> I = std::complex<double>(0.0, 1.0);

const int nState = 7;
const double maxP = 6;
typedef Eigen::Matrix<double, nState, 1> Feature;

class Lattice {

  const double V0 = 5;
  Eigen::VectorXd p;
  Eigen::MatrixXd H0;
  Eigen::MatrixXcd H1;
  Eigen::MatrixXcd H2;
  Eigen::VectorXcd w;
  Eigen::MatrixXcd v;

  Eigen::VectorXcd psi;
  Eigen::VectorXcd target;

public:

  Lattice()
  {
    p = Eigen::VectorXd::LinSpaced(nState, -maxP, maxP);
    H0 = (p.array().square() / 2).matrix().asDiagonal();
    H1 = Eigen::MatrixXcd::Zero(nState, nState); // sin
    H1.block(0, 1, nState - 1, nState - 1) += V0 / 4
      * I * Eigen::MatrixXcd::Identity(nState - 1, nState - 1);
    H1.block(1, 0, nState - 1, nState - 1) -= V0 / 4
      * I * Eigen::MatrixXcd::Identity(nState - 1, nState - 1);
    H2 = Eigen::MatrixXcd::Zero(nState, nState); // cos
    H2.block(0, 1, nState - 1, nState - 1) += V0 / 4
      * Eigen::MatrixXcd::Identity(nState - 1, nState - 1);
    H2.block(1, 0, nState - 1, nState - 1) += V0 / 4
      * Eigen::MatrixXcd::Identity(nState - 1, nState - 1);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> eigensolver(H0 - H2);
    w = eigensolver.eigenvalues();
    v = eigensolver.eigenvectors();
    psi = v.col(0);
    target = v.col(3);
  }

  void groundState()
  {
    psi = v.col(0);
  }

  double fidelity() const
  {
    return (psi.adjoint() * target).squaredNorm();
  }

  Feature feature() const
  {
    Eigen::VectorXcd result(psi);

    for (long i = 0; i < psi.size() / 2; i++) {
      int idx = psi.size() - i - 1;
      result[i] = 0.5 * (psi[i] + psi[idx]);
      result[idx] = 0.5 * (psi[i] - psi[idx]);
    }

    return result.cwiseAbs2();
  }

  void step(double phi, double dt)
  {
    Eigen::MatrixXcd H = H0 + sin(phi) * H1 - cos(phi) * H2;
    psi = (-I * H * dt).exp() * psi;
  }
};

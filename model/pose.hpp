#pragma once

class Quaternion
{
public:
    double w, x, y, z;
    Quaternion(double, double, double, double);
    Quaternion operator~() const;
    Quaternion operator-() const;
    Quaternion operator+(const Quaternion&) const;
    Quaternion operator-(const Quaternion&) const;
    Quaternion operator*(const Quaternion&) const;
    Quaternion operator*(double) const;
};

class PoseTransform
{
public:
    Quaternion rotation, translation;
    PoseTransform(const Quaternion&, const Quaternion&);
    PoseTransform(const double[3]);
    PoseTransform(const double[3], const double[3]);
    PoseTransform operator*(const PoseTransform&) const;
    Quaternion operator*(const Quaternion&) const;
};
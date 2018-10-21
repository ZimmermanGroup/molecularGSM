#if 0
  ofstream grad_file;
  string grad_file_string = "grad_file.txt";
  grad_file.open(grad_file_string.c_str());
  grad_file.setf(ios::fixed);
  grad_file.setf(ios::left);
  grad_file << setprecision(6);

  ofstream tan_grad_file;
  string tan_grad_file_string = "tan_grad_file.txt";
  tan_grad_file.open(tan_grad_file_string.c_str());
  tan_grad_file.setf(ios::fixed);
  tan_grad_file.setf(ios::left);
  tan_grad_file << setprecision(6);

  ofstream perp_grad_file;
  string perp_grad_file_string = "perp_grad_file.txt";
  perp_grad_file.open(perp_grad_file_string.c_str());
  perp_grad_file.setf(ios::fixed);
  perp_grad_file.setf(ios::left);
  perp_grad_file << setprecision(6);

  ofstream Vfile;
  string Vfile_string = "Vfile.txt";
  Vfile.open(Vfile_string.c_str());
  Vfile.setf(ios::fixed);
  Vfile.setf(ios::left);
  Vfile << setprecision(6);

  ofstream Vfile_kcal;
  string Vfile_kcal_string = "Vfile_kcal.txt";
  Vfile_kcal.open(Vfile_kcal_string.c_str());
  Vfile_kcal.setf(ios::fixed);
  Vfile_kcal.setf(ios::left);
  Vfile_kcal << setprecision(6);
#endif

  ofstream SVfile;
  string newSVfile;
  SVfile.setf(ios::fixed);
  SVfile.setf(ios::left);
  SVfile << setprecision(6);

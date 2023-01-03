#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <opencv2/core/types_c.h>
#include <cstdlib>
#include <math.h>
#include <vector>

#include <fstream>

using namespace cv;
using namespace std;


//seuillage
void seuillage(int n, Mat image, Mat aide) {

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (image.at<uchar>(i, j) > n) {
                aide.at<uchar>(i, j) = 255;
            }

        }
    }
}

//Dessin de l'histogramme avec sa taille et un tableau pour les vals
void histogramme(Mat image, int hist[], int taille) {

    for (int i = 0; i < taille; i++) {
        hist[i] = 0;
    }

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            hist[image.at<Vec3b>(i, j)[0]]++;
        }


    }


}

int max1(int tab[], int taille) {

    int max = tab[0];
    for (int i = 0; i < taille; i++) {
        if (max < tab[i]) {
            max = tab[i];
        }

    }
    return max;
}

void afficheTab(int tab[]) {

    for (int i = 0; i < 256; i++) {
        cout << " tab " << i << " : " << tab[i] << endl;

    }

}


Mat  CC(Mat img) {
    Mat labelImage(img.size(), CV_32S);

    // nombre de CC dans l'image seuillée
    int nLabels = connectedComponents(img, labelImage, 4);


    // fixer la couleur pour chaque CC
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
    }
    Mat dst(img.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {

            int label = labelImage.at<int>(r, c);
            Vec3b& pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }

    return dst;

}
int maxTab(vector <int> tab, int n) {
    int max = tab[0];
    for (int i = 0; i < n; i++) {
        if (max < tab[i]) {
            max = tab[i];
        }
    }
    return max;
}

int minTab(vector <int> tab, int n) {
    int min = maxTab(tab, n), max = maxTab(tab, n);

    // cout<<"min : "<<min<<endl;
    for (int i = 0; i < n; i++) {
        if (min > tab[i] && tab[i] != 0) {
            min = tab[i];
        }
    }
    return max - min;
}

int debut(vector <int> tab, int n, int m) {
    int debut = tab[0];
    bool choix = true;
    int i = 0;
    while (choix && i < n) {
        if (tab[i] != 0 && tab[i] > m) {
            debut = i;
            // cout<<tab[i]<<" tab debut"<<endl;
            choix = false;
            break;
        }
        i++;
    }
    return debut;
}
int fin(vector <int> tab, int n, int m) {
    int fin = tab[0];
    bool choix = true;
    int i = n - 1;
    while (choix && i > 0) {
        if (tab[i] != 0 && tab[i] > m) {
            fin = i;
            choix = false;
            break;
        }
        i--;
    }
    return fin;
}



int main() {
    /*******************avoir une image ************************/
    Mat img;
    img = imread("C:\\Users\\Adams\\Desktop\\projet_intro_image-main\\projet_image\\Image2.png", IMREAD_COLOR);
    imshow("Image non traitée", img);
    cout << " Ligne : " << img.rows << ",Colonne : " << img.cols << endl;

    /**************Avoir une image en niveau de gris ****************/
    Mat nivGris(img.size(), CV_8UC1);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            nivGris.at<uchar>(i, j) = (img.at<Vec3b>(i, j)[0] + img.at<Vec3b>(i, j)[1] + img.at<Vec3b>(i, j)[2]) / 3;
            /* nivGris.at<Vec3b>(i,j)[0]=(img.at<Vec3b>(i,j)[0]+img.at<Vec3b>(i,j)[1]+img.at<Vec3b>(i,j)[2])/3;
              nivGris.at<Vec3b>(i,j)[1]=(img.at<Vec3b>(i,j)[0]+img.at<Vec3b>(i,j)[1]+img.at<Vec3b>(i,j)[2])/3;
              nivGris.at<Vec3b>(i,j)[2]=(img.at<Vec3b>(i,j)[0]+img.at<Vec3b>(i,j)[1]+img.at<Vec3b>(i,j)[2])/3;*/
        }
    }
    imshow("Niveaux de gris", nivGris);
    // waitKey(0);


   /*********************contour de l'image **************************/

    Mat dst;
    Canny(img, dst, 50, 200, 3);
    imshow("Canny image", dst);

    waitKey(0);


    /*******************detection du verre **********************/


    // Detecte des contours sur l'image binaire
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(/*~thresh*/dst, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
    // Dessine des contours sur l'image originale
    Mat image_copy = img.clone();
    Mat clone = Mat::zeros(img.size(), CV_8UC3);

    drawContours(image_copy, contours, -1, Scalar(0, 255, 0), 2);
    drawContours(clone, contours, -1, Scalar(0, 255, 0), 2);

    imshow("Clone", clone);
    waitKey(0);

    /***********************taille du verre ***************************/

    int haut = 0, bas = 0, gauche = 0, droite = 0;
    int mLigne = 0, mColonne = 0;
    std::vector<int> tabligne(img.rows);
    std::vector<int> tabcolonne(img.cols);

    Mat Lignes = Mat::zeros(img.size(), CV_8UC1);
    Mat Colonnes = Mat::zeros(img.size(), CV_8UC1);
    for (int i = 0; i < img.rows; i++) {
        int lignes = 0;
        for (int j = 0; j < img.cols; j++) {
            if (clone.at<Vec3b>(i, j)[0] == 0 && clone.at<Vec3b>(i, j)[1] == 255 && clone.at<Vec3b>(i, j)[2] == 0) {
                lignes++;
            }

        }

        tabligne[i] = lignes;
        //cout<<lignes<<"\t";
        for (int x = 0; x < lignes; x++) {
            Lignes.at<uchar>(i, x) = 255;
        }
    }
    //cout<<endl;
    for (int i = 0; i < img.cols; i++) {
        int colonne = 0;
        for (int j = 0; j < img.rows; j++) {
            if (clone.at<Vec3b>(j, i)[0] == 0 && clone.at<Vec3b>(j, i)[1] == 255 && clone.at<Vec3b>(j, i)[2] == 0) {
                colonne++;
            }

        }

        tabcolonne[i] = colonne;
        //cout<<colonne<<"\t";
        for (int x = 0; x < colonne; x++) {
            Colonnes.at<uchar>(x, i) = 255;
        }
    }
    //cout<<endl;


    mLigne = minTab(tabligne, img.rows) / 4;
    mColonne = minTab(tabcolonne, img.cols) / 4;
    //cout<<mLigne<< " "<<mColonne<<endl;

    haut = debut(tabligne, img.rows, mLigne);
    bas = fin(tabligne, img.rows, mLigne);
    gauche = debut(tabcolonne, img.cols, mColonne);
    droite = fin(tabcolonne, img.cols, mColonne);
    cout << " Haut : " << haut << " , Bas : " << bas << ", Gauche : " << gauche << ",Droite : " << droite << endl;



    imshow("Lignes", Lignes);
    imshow("Colonnes", Colonnes);
    waitKey(0);


    /************Les contours du verre**********************/
    Mat verre = Mat::zeros(img.size(), CV_8UC1);
    for (int i = haut; i < bas; i++) {
        for (int j = gauche; j < droite; j++) {
            if (clone.at<Vec3b>(i, j)[1] == 255) {
                verre.at<uchar>(i, j) = 255;
            }

        }
    }

    imshow("Verre", verre);

    waitKey(0);


    /********************Ouverture et fermeture************************/
    Mat dilatation = Mat::zeros(verre.size(), CV_8UC1);

    dilate(/* c*//*aidecontour*/verre, dilatation, Mat(), Point(-1, -1), 3);
    erode(dilatation, dilatation, Mat(), Point(-1, -1), 2);
    imshow("dilat", dilatation);
    dilatation.convertTo(dilatation, CV_8UC1);

    //cout<<dilatation.type()<<endl;

    /**************Tracage des lignes ****************/
    Mat lcontour = Mat::zeros(verre.size(), CV_8UC1);
    Mat rcontour = Mat::zeros(verre.size(), CV_8UC3);


    Mat milieu = dilatation.clone();

    for (int i = 0; i < dilatation.rows; i++) {
        for (int j = 0; j < gauche; j++) {
            milieu.at<uchar>(i, j) = 0;
            if ((j + gauche + (droite - gauche)) < dilatation.cols) {
                milieu.at<uchar>(i, j + gauche + (droite - gauche)) = 0;
            }


        }

    }
    imshow("Milieu", milieu);

    /*******************histogramme de projection entre intervalles *******************/
   // cout<<"img.cols"<<verre.cols<<"\n"<<verre.cols/2-50<<" "<<verre.cols/2+50<<endl;
    cout << "avant g et d " << endl;
    int g = verre.cols / 2 - 40, d = verre.cols / 2 + 40;
    cout << g << " " << d << endl;
    if ((droite - gauche / 2) > 40) {
        if ((gauche + (droite - gauche) / 2 - 40) > 0) {
            g = gauche + (droite - gauche) / 2 - 40;
        }

        d = droite;

    }
    cout << g << " " << d << endl;
    for (int i = 0; i < verre.rows; i++) {
        int blanc = 0;
        for (int j = g; j < d; j++) {
            if (/*aidecontour*/dilatation.at<uchar>(i, j) == 255) {
                // if(erode.at<uchar>(i,j)==255){
                blanc++;
            }
        }

        // cout<<"blanc  : "<<blanc<<endl;
         //contour filtre pour img.cols/8
         //egalisation img.cols/3
        if (blanc > 50) {

            for (int j = 0; j < verre.cols; j++) {
                rcontour.at<Vec3b>(i, j)[0] = 255;
                rcontour.at<Vec3b>(i, j)[1] = 255;
                rcontour.at<Vec3b>(i, j)[2] = 255;
            }
        }
        else {
            for (int j = 0; j < img.cols; j++) {
                rcontour.at<Vec3b>(i, j)[0] = 0;
                rcontour.at<Vec3b>(i, j)[1] = 0;
                rcontour.at<Vec3b>(i, j)[2] = 0;
            }
        }
        for (int j = 0; j < blanc; j++) {
            lcontour.at<uchar>(i, j) = 255;
        }

    }


    imshow("Avoir les contours blancs", lcontour);
    imshow("Avoir les contours traités", rcontour);
    waitKey(0);

    /**********Lignes de segmentation *******************/

    vector<int> lignes;
    for (int i = 1; i < verre.rows - 1; i++) {
        if (i != 0 || i != verre.rows) {

            if (rcontour.at<uchar>(i - 1, 0) == 0 && rcontour.at<uchar>(i, 0) == 255 && rcontour.at<uchar>(i + 1, 0) == 255) {
                lignes.push_back(i);
            }

            if (rcontour.at<uchar>(i - 1, 0) == 255 && rcontour.at<uchar>(i, 0) == 255 && rcontour.at<uchar>(i + 1, 0) == 0) {
                lignes.push_back(i);
            }
        }

    }
    

    vector<int> nLignes;
    vector<int> newLignes;
    for (int i = 0; i < lignes.size() - 1; i = i + 2) {
        if (lignes[i + 1] - lignes[i] > 1 && (i + 1) < lignes.size()) {

            nLignes.push_back(lignes[i]);
            nLignes.push_back(lignes[i + 1]);
            //cout<<lignes[i]<<" "<<lignes[i+1]<<endl;
        }

    }
    for (int i = 0; i < nLignes.size(); i = i + 2) {
        if (i + 1 < nLignes.size()) {
            newLignes.push_back(nLignes[i + 1] - nLignes[i]);
        }


    }

   
    int maxLigneblanc = 0;
    if (newLignes.size() > 2) {
        maxLigneblanc = newLignes[1];
    }
    int indice = 0;

    for (int i = 1; i < newLignes.size() - 1; i++) {
        //if(i+1<newLignes.size()-1){
        if (maxLigneblanc < newLignes[i] /*&&newLignes[i]>nLignes[i+1]*/) {
            maxLigneblanc = newLignes[i];
            indice = i;
        }
        //}

    }
    if (indice == 0) {
        if (newLignes.size() >= 3) {
            indice = 1;
        }
    }
    cout << "Max ligne blanche dans le 'milieu eau' : " << maxLigneblanc << " , indice : " << indice << endl;
    float tailleVerre = nLignes[(nLignes.size() - 1)] - nLignes[0];
    float tailleEau = 0;
    cout << "Taille du verre : " << tailleVerre << endl;
    cvtColor(verre, verre, COLOR_GRAY2RGB);
    // tailleEau=newLignes[(newLignes.size()-2)]*100/tailleVerre;
    if (maxLigneblanc == 0) { //Voudrait dire que le niveau de l'eau == haut du verre.
        cout << "Verre trop rempli ou vide." << endl;
    }
    else {
        for (int i = nLignes[indice * 2]; i < nLignes[nLignes.size() - 2]; i++) {
            for (int j = gauche; j < droite; j++) {
                verre.at<Vec3b>(i, j)[0] = 255;
                //cout<<"color"<<endl;    
            }

        }
        tailleEau = (nLignes[nLignes.size() - 2] - nLignes[indice * 2]) * 100 / tailleVerre;

    }

    for (int j = 0; j < verre.cols; j++) {
        verre.at<Vec3b>(haut, j)[2] = 255;
        verre.at<Vec3b>(bas, j)[2] = 255;
    }

    cout << "taille eau : " << tailleEau << endl;
    imshow("verre avec l'eau", verre);
    waitKey(0);

    /****************Vérité terrain*****************/

    ifstream image_file("C:\\Users\\Adams\\Desktop\\projet_intro_image-main\\projet_image\\image2.json");
    string ligne;
    vector<string> v1;

    vector<string>v3;
    vector<int>v4;

    int nombre = 8;
    while (getline(image_file, ligne)) {
        //cout<<ligne<<endl;
         //cout<<"taille"<<ligne.size()<<endl;
        if (ligne.find("label") != string::npos) {
            v1.push_back(ligne);
            // cout<<ligne.find("label")<<endl;;
        }
        if (ligne.find("points") != string::npos) {
            string l;
            int number = 0;
            while (getline(image_file, l) && number < nombre) {
                v3.push_back(l);
                number++;

            }

        }

    };

    for (int i = 0; i < v1.size(); i++) {
        cout << v1[i] << endl;
    }

    for (int i = 0; i < v3.size(); i++) {
        //cout<<v3[i]<<endl;
        try {
            int num2 = stoi(v3[i]);
            // cout << "num2: " << num2 << endl;
            v4.push_back(num2);
        }
        catch (std::exception& e) {
            //cout << e.what() << " caused the error!!!\n";
        }
    }

    cout << "taille de v4 " << v4.size() << endl;

    for (int i = 0; i < v4.size(); i = i + 4) {
        /* int x1=v4[i]
         int y1=v4[i+1]
         int x2=v4[i+2]
         int y2=v4[i+3]*/
        line(verre, Point(v4[i], v4[i + 1]), Point(v4[i + 2], v4[i + 3]), Scalar(0, 255, 0), 1, LINE_4);


    }


    imshow("verre avec l'eau", verre);
    waitKey(0);

    return 0;
}
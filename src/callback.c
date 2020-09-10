#include "callback.h"

void integration(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col)
{
    scalar *U = accessArray(dataSet_vertex, 0);
    scalar *flux = accessArray(dataSet_edge, 0);
    // here dim_U = dim_flux
    label dim_U = getArrayDim(dataSet_vertex, 0);
    label dim_flux = getArrayDim(dataSet_edge, 0);
    label n_edge = getArraySize(dataSet_edge);

    int i,j;
    for (i = 0; i < n_edge; ++i)
    {
        for (j = 0; j < dim_U; ++j)
        {
            U[row[i]*dim_U+j] += flux[i*dim_U+j];
            U[col[i]*dim_U+j] += flux[i*dim_U+j];
        }
    }
}

void spMV(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col)
{
    scalar *x = accessArray(dataSet_vertex, 0);
    scalar *b = accessArray(dataSet_vertex, 1);
    scalar *A = accessArray(dataSet_edge, 0);
    // here dim_U = dim_flux
    label n_edge = getArraySize(dataSet_edge);

    int i;
    for (i = 0; i < n_edge; ++i)
    {
        b[col[i]] += A[i]*x[row[i]];
        b[row[i]] += A[i]*x[col[i]];
    }
}

void compVisflux(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col)
{
    int i,j;
    scalar* xfc                   = accessArray(dataSet_edge, 0);
    scalar* yfc                   = accessArray(dataSet_edge, 1);
    scalar* zfc                   = accessArray(dataSet_edge, 2);
    scalar* xfn                   = accessArray(dataSet_edge, 3);
    scalar* yfn                   = accessArray(dataSet_edge, 4);
    scalar* zfn                   = accessArray(dataSet_edge, 5);
    scalar* primitiveVariableFace = accessArray(dataSet_edge, 6);
    scalar* tm                    = accessArray(dataSet_edge, 7);
    scalar* deltaL                = accessArray(dataSet_edge, 8);
    scalar* deltaR                = accessArray(dataSet_edge, 9);
    scalar* viscousLaminarFace    = accessArray(dataSet_edge, 10);
    scalar* viscousTurbulentFace  = accessArray(dataSet_edge, 11);
    scalar* kCp                   = accessArray(dataSet_edge, 12);
    scalar* flux                  = accessArray(dataSet_edge, 13);
    scalar* area                  = accessArray(dataSet_edge, 14);

    scalar* xcc               = accessArray(dataSet_vertex, 0);
    scalar* ycc               = accessArray(dataSet_vertex, 1);
    scalar* zcc               = accessArray(dataSet_vertex, 2);
    scalar* primitiveVariable = accessArray(dataSet_vertex, 3);
    scalar* t                 = accessArray(dataSet_vertex, 4);
    scalar* dqdx              = accessArray(dataSet_vertex, 5);
    scalar* dqdy              = accessArray(dataSet_vertex, 6);
    scalar* dqdz              = accessArray(dataSet_vertex, 7);
    scalar* dtdx              = accessArray(dataSet_vertex, 8);
    scalar* dtdy              = accessArray(dataSet_vertex, 9);
    scalar* dtdz              = accessArray(dataSet_vertex, 10);

    label nEquation = getArrayDim(dataSet_vertex, 3);

    label nface = getArraySize(dataSet_edge);

    scalar t1x, t1y, t1z, t2x, t2y, t2z;
    scalar txx,tyy,tzz;
    scalar txy,txz,tyz;

    scalar fMid[nEquation], f1[nEquation], f2[nEquation];
    scalar dfdn[nEquation], dfd1[nEquation], dfd2[nEquation];
    scalar dfdt1[nEquation], dfdt2[nEquation];
    scalar dfdx[nEquation], dfdy[nEquation], dfdz[nEquation];
    scalar fvis[5];

    for (i = 0; i < nface; ++i)
    {
        label le = row[i];
        label re = col[i];

        scalar nxs = xfn[i];
        scalar nys = yfn[i];
        scalar nzs = zfn[i];
    
        //! Get first tangential vector on the face.
        if (fabs(nxs) > SMALL)
        {
            t1x =   nys;
            t1y = - nxs;
            t1z =   0.0;
        }
        else if (fabs(nys) > SMALL)
        {
            t1x = - nys;
            t1y =   nxs;
            t1z =   0.0;
        }
        else
        {
            t1x =   0.0;
            t1y = - nzs;
            t1z =   nys;
        }

        //! Normalize the tangential vector.
        // scalar oNormal = 1.0 / DISTANCE(t1x, t1y, t1z);
        scalar oNormal = 1.0 / sqrt(t1x*t1x+t1y*t1y+t1z*t1z);
        t1x *= oNormal;
        t1y *= oNormal;
        t1z *= oNormal;
    
        //! Get the second tangential vector by cross dot t1 to normal.
        t2x = nys * t1z - nzs * t1y;
        t2y = nzs * t1x - nxs * t1z;
        t2z = nxs * t1y - nys * t1x;

        scalar dxL = xcc[le] - xfc[i];
        scalar dyL = ycc[le] - yfc[i];
        scalar dzL = zcc[le] - zfc[i];
    
        scalar dxR = xcc[re] - xfc[i];
        scalar dyR = ycc[re] - yfc[i];
        scalar dzR = zcc[re] - zfc[i];

        scalar dL = nxs * dxL + nys * dyL + nzs * dzL;
        scalar dR = nxs * dxR + nys * dyR + nzs * dzR;

        scalar distTemp1 = - dL / (sqrt(dxL*dxL+dyL*dyL+dzL*dzL) + SMALL);
        distTemp1 = MIN(distTemp1,  1.0);
        distTemp1 = MAX(distTemp1, -1.0);
        
        scalar distTemp2 = dR / (sqrt(dxR*dxR+dyR*dyR+dzR*dzR) + SMALL);
        distTemp2 = MIN(distTemp2,  1.0);
        distTemp2 = MAX(distTemp2, -1.0);

        scalar dxnL  = nxs * dL - dxL;
        scalar dynL  = nys * dL - dyL;
        scalar dznL  = nzs * dL - dzL;
        
        scalar dxnR  = nxs * dR - dxR;
        scalar dynR  = nys * dR - dyR;
        scalar dznR  = nzs * dR - dzR;
        
        //! Quantities at points 1 and 2.
        for (j = 0; j < nEquation; ++j)
        {
            f1[j] = primitiveVariable[le*nEquation+j];
            f2[j] = primitiveVariable[re*nEquation+j];
        }
        
        scalar tL = t[le];
        scalar tR = t[re];
        
        for (j = 0; j < nEquation; ++ j)
        {
            fMid[j] = 0.5 * (f1[j] + f2[j]);
        }
        scalar tMid = 0.5 * (tL + tR);

        if (distTemp1 > 0.866 && distTemp2 > 0.866)
        {
            for (j = 0; j < nEquation; ++ j)
            {
                f1[j] += dqdx[j+le*nEquation] * dxnL 
                      +  dqdy[j+le*nEquation] * dynL
                      +  dqdz[j+le*nEquation] * dznL;
                f2[j] += dqdx[j+re*nEquation] * dxnR
                      +  dqdy[j+re*nEquation] * dynR
                      +  dqdz[j+re*nEquation] * dznR;
            }
    
            tL += dtdx[le] * dxnL + dtdy[le] * dynL + dtdz[le] * dznL;
            tR += dtdx[re] * dxnR + dtdy[re] * dynR + dtdz[re] * dznR;
    
            if (tL < SMALL)
            {
                tL = t[le];
            }
            if (tR < SMALL)
            {
                tR = t[re];
            }
    
            //! Quantities at the face.
            for (j = 0; j < nEquation; ++ j)
            {
                fMid[j] = primitiveVariableFace[j+i*nEquation];
            }
    
            tMid = tm[i];
        }

        for (j = 0; j < nEquation; ++ j)
        {
            dfdn[j] = 0.0;
        }
        scalar dtdn = 0.0;

        if (distTemp1 > 0.0 && distTemp2 > 0.0 && fabs(dL) > SMALL && fabs(dR) > SMALL)
        {
            for (j = 0; j < nEquation; ++ j)
            {
                dfd1[j] = (f1[j] - fMid[j]) / dL;
            }
    
            for (j = 0; j < nEquation; ++ j)
            {
                dfd2[j] = (f2[j] - fMid[j]) / dR;
            }
    
            scalar dtd1 = (tL - tMid) / dL;
            scalar dtd2 = (tR - tMid) / dR;
    
            scalar dtmp = dL * dL + dR * dR;
            scalar weightL = dL * dL / dtmp;
            scalar weightR = 1.0 - weightL;

            for (j = 0; j < nEquation; ++ j)
            {
                dfdn[j] = weightL * dfd1[j] + weightR * dfd2[j];
            }
    
            dtdn = dtd1 * weightL + dtd2 * weightR;
        }

        scalar weightL = deltaL[i];
        scalar weightR = deltaR[i];
    
        for (j = 0; j < nEquation; ++ j)
        {
            dfdx[j] = weightL * dqdx[j+le*nEquation]
                    + weightR * dqdx[j+re*nEquation];
            dfdy[j] = weightL * dqdy[j+le*nEquation]
                    + weightR * dqdy[j+re*nEquation];
            dfdz[j] = weightL * dqdz[j+le*nEquation]
                    + weightR * dqdz[j+re*nEquation];
        }
    
        for (j = 1; j <= 3; ++ j)
        {
            dfdt1[j] = t1x * dfdx[j] + t1y * dfdy[j] + t1z * dfdz[j];
            dfdt2[j] = t2x * dfdx[j] + t2y * dfdy[j] + t2z * dfdz[j];
        }
    
        //! Now true gradients. 
        for (j = 1; j <= 3; ++ j)
        {
            dfdx[j] = nxs * dfdn[j] + t1x * dfdt1[j] + t2x * dfdt2[j];
            dfdy[j] = nys * dfdn[j] + t1y * dfdt1[j] + t2y * dfdt2[j];
            dfdz[j] = nzs * dfdn[j] + t1z * dfdt1[j] + t2z * dfdt2[j];
        }

        scalar dudx = dfdx[1];
        scalar dudy = dfdy[1];
        scalar dudz = dfdz[1];
    
        scalar dvdx = dfdx[2];
        scalar dvdy = dfdy[2];
        scalar dvdz = dfdz[2];
    
        scalar dwdx = dfdx[3];
        scalar dwdy = dfdy[3];
        scalar dwdz = dfdz[3];
    
        scalar qNorm = 0.0;
        
        qNorm += kCp[i] * dtdn;
        scalar divv2p3 = 2.0/3.0 * (dudx + dvdy + dwdz);
    
        scalar vis = viscousLaminarFace[i] + viscousTurbulentFace[i];
        scalar um  = fMid[1];
        scalar vm  = fMid[2];
        scalar wm  = fMid[3];
    
        //! Stress components.
        txx = vis * (2 * dudx - divv2p3);
        tyy = vis * (2 * dvdy - divv2p3);
        tzz = vis * (2 * dwdz - divv2p3);
        txy = vis * (dudy + dvdx);
        txz = vis * (dudz + dwdx);
        tyz = vis * (dvdz + dwdy);

        fvis[0] = 0.0;
        fvis[1] = nxs * txx + nys * txy + nzs * txz;
        fvis[2] = nxs * txy + nys * tyy + nzs * tyz;
        fvis[3] = nxs * txz + nys * tyz + nzs * tzz;
        fvis[4] = um * fvis[1] + vm * fvis[2] + wm * fvis[3] + qNorm;

        for (j = 0; j < 5; ++ j )
        {
            flux[j+i*5] = - area[i] * fvis[j];
        }
    }
}

void calcLudsFcc(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col)
{
    scalar *massFlux = accessArray(dataSet_edge, 0);
    scalar *facex    = accessArray(dataSet_edge, 1);
    scalar *fcc      = accessArray(dataSet_edge, 2);
    scalar *rface0   = accessArray(dataSet_edge, 3);
    scalar *rface1   = accessArray(dataSet_edge, 4);

    scalar *cellx   = accessArray(dataSet_vertex, 0);
    scalar *S       = accessArray(dataSet_vertex, 1);

    label dim_massFlux = getArrayDim(dataSet_edge, 0);
    label dim_facex    = getArrayDim(dataSet_edge, 1);
    label dim_fcc      = getArrayDim(dataSet_edge, 2);
    label dim_rface0   = getArrayDim(dataSet_edge, 3);
    label dim_rface1   = getArrayDim(dataSet_edge, 4);

    label dim_cellx = getArrayDim(dataSet_vertex, 0);
    label dim_S     = getArrayDim(dataSet_vertex, 1);

    label n_edge = getArraySize(dataSet_edge);

    int i,j;
    scalar facp, facn;
    for (i = 0; i < n_edge; ++i)
    {
        facp = massFlux[i*dim_massFlux+0] >= 0.0 ? massFlux[i*dim_massFlux+0] : 0.0;
        facn = massFlux[i*dim_massFlux+0] <  0.0 ? massFlux[i*dim_massFlux+00] : 0.0;
        fcc[i*dim_fcc+0] = facn*(facex[i*dim_facex+0]-cellx[col[i]*dim_cellx+0])
                         + facp*(facex[i*dim_facex+0]-cellx[row[i]*dim_cellx+0]);
        rface0[i*dim_rface0+0] = facp;
        rface1[i*dim_rface1+0] = facn;
        S[row[i]*dim_S+0]      += facp;
        S[col[i]*dim_S+0]      -= facn;
    }
}
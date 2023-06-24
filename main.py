
import csv
import pandas as pd
import seaborn as sns
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from keras import backend as K
from scipy import optimize

import sklearn

data_path = "largefile.txt"
print(data_path)
def mae(y_test, y_pred):
    """
    Calculuates mean absolute error between y_test and y_preds.
    """
    return tf.metrics.mean_absolute_error(y_test,
                                          y_pred)

def mse(y_test, y_pred):
    """
    Calculates mean squared error between y_test and y_preds.
    """
    return tf.metrics.mean_squared_error(y_test,
                                         y_pred)

# begin data reading
heart_disease =  pd.read_csv(data_path, header=None, delim_whitespace=True)
heart_disease.replace([np.inf, -np.inf], np.nan, inplace=True)
# Drop rows with NaN
heart_disease.dropna(inplace=True)

#print(heart_disease)

x1 = heart_disease.iloc[:, 0:10].values
y1 = heart_disease.iloc[:, 11:12].values
x1=np.delete(x1,4,axis=1)
x1=np.delete(x1,8,axis=1)

#indices_permutation = np.random.permutation(len(x1))
x = x1#x1[indices_permutation]
y = y1#y1[indices_permutation]

yflat=y.flatten().tolist()

indmax=yflat.index(max(yflat))
print("indmax=")
print(indmax)
xbest=x[indmax,0:8]
print("xbest=")
print(xbest)
ybest=max(yflat)
print("ybest=")
print(ybest)
xmin=[0,0,0,0,0,0,0,0]
xmax=[0,0,0,0,0,0,0,0]

for i in range(8):
    xmin[i] = np.min(x[:,i])-1.0e-9
    xmax[i]=  np.max(x[:,i])+1.0e-9

print("xmin=")
print(xmin)

print("xmax=")
print(xmax)
print(x)
print(y)
# ends data reading

from sklearn.model_selection import train_test_split

x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.04, random_state=0)

x_train.shape, x_test.shape, y_train.shape, y_test.shape

import keras
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

model = tf.keras.Sequential([
    Dense(units=1028, kernel_initializer='he_uniform', activation='relu', input_dim=8),
    Dense(400),
    Dense(200),
    Dense(100),
    Dense(10),
    Dense(1)])
# Dense(units=1, kernel_initializer = 'he_uniform', activation = 'relu')])

model.compile(loss='mae', optimizer='adam', metrics=['mae'])

history = model.fit(x_train, y_train, epochs=6000, batch_size=16, verbose=2)

y_preds = model.predict(x_test)  # posso meter aqui parametros do perfil nunca vistos para prever o resultado

y_preds.shape, y_test.shape, y_preds[:5], y_test[:5]

# Plot history (also known as a loss curve)
pd.DataFrame(history.history).plot()
plt.ylabel("loss")
plt.xlabel("epochs")
plt.show()

#grads = K.gradients(model.output, model.input)

# Calculate the mean absolute error
mae = tf.metrics.mean_absolute_error(y_true=y_test.squeeze(),
                                     y_pred=y_preds.squeeze())
mae

# Save a model using the SavedModel format
model.save('best_model_SavedModel_format_0')

zz = x


def gradienteR(xx,model):
    for i in range(8):
        xx[i] = max(xmin[i], min(xmax[i], xx[i]))
        

    x=[xx]
    x = list(x)
    x = tf.convert_to_tensor(x)
    x = tf.constant(x)
    with tf.GradientTape() as t:
        t.watch(x)
        y = model(x)
        r = t.gradient(y, x)
        r=r.numpy()
        r=r.flatten()
        gfk_norm = np.linalg.norm(r)
        print("norm=")
        print(gfk_norm)
        if gfk_norm<1.0e-20:
            print(xx)
            print("error")
        return -r

def modelR(xx,model):
#    for i in range(8):
#       xx[i] = max(xmin[i], min(xmax[i], xx[i]))       
    x=[xx.copy()]
    x=list(x)


    x = tf.constant(x)
    x = tf.convert_to_tensor(x)

    y = model(x)
    y= y.numpy()
    y=y.flatten()
    result=y.item(0)
#    if result<0:
#        result=-1.0e20
#    for i in range(4):
#        if xx[i]>=xx[5+i]:
#            result=-1.0e20
    return -result

print("inicion")

print(modelR(xarb,model))
values= [None] * 20
xar=xarb
for i in range(20):
    xar[0]=xmin[0]*(i/20.0)+xmax[0]*(1.0e00-i/20.0)
    values[i]=modelR(xar,model)
print(xar)
print(values)
print("fiiiiiim")
from scipy import optimize


print("BOUNDS=")
bounds=[[xmin[0],xmax[0]],[xmin[1],xmax[1]],[xmin[2],xmax[2]],[xmin[3],xmax[3]],[xmin[4],xmax[4]],[xmin[5],xmax[5]],[xmin[6],xmax[6]],[xmin[7],xmax[7]]]
print(bounds)

xarb=xbest
print("xbest=")
print(xbest)
print("ybest=")
print(ybest)
#res0 = optimize.fmin_cg(modelR, x0=xarb,fprime=gradienteR,args=[model],gtol=1e-9,norm=np.Inf,epsilon=1.0e-4,maxiter=100,full_output=True)
#res0 = optimize.fmin_cg(modelR, x0=xarb,args=[model],gtol=1e-9,norm=np.Inf,epsilon=1.0e-3,maxiter=100,full_output=True)

#print(res0)
#print("x=")
#print(res0[0])
#l1=res0[0].tolist()
#l1=np.insert(l1,4,0)
#l1=np.insert(l1,9,0)

#with open('parametersopt2.txt','w') as file1:
#    for a in l1:
#        file1.write(f"{a}")
#        file1.write(" ")

#res1=optimize.differential_evolution(modelR, bounds, args=[model],maxiter=1230000,tol=1.0e-9,polish=False,atol=1.0e-9,x0=xbest,updating='immediate',strategy='randtobest1bin')
#print(res1)
#print("x=")
#print(res1.x)
#l1=res1.x.tolist()
#with open('parametersopt1.txt','w') as file1:
#    for a in l1:
#        file1.write(f"{a}")
#        file1.write(" ")

res2=optimize.dual_annealing(modelR, bounds,x0=xarb, args=[model],maxiter=12000,initial_temp=1000,maxfun=100000,visit=10.00)
#res2=optimize.differential_evolution(modelR, bounds, args=[model],maxiter=1230000,tol=1.0e-9,polish=False,atol=1.0e-9,x0=xbest,updating='immediate',strategy='randtobest1bin')
print(res2)
print("x=")
print(res2.x)
l1=res2.x.tolist()
l1=np.insert(l1,4,0)
l1=np.insert(l1,9,0)
with open('parametersopt.txt','w') as file1:
    for a in l1:
        file1.write(f"{a}")
        file1.write(" ")

        
print("xbest=")
print(xbest)
print("fbest=")
print(modelR(xbest,model))

l1=xbest #.tolist()
l1=np.insert(l1,4,0)
l1=np.insert(l1,9,0)
with open('parametersbest.txt','w') as file1:
    for a in l1:
        file1.write(f"{a}")
        file1.write(" ")
